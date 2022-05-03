import fs from 'fs';
import Axios from 'axios';
import type { AxiosInstance } from 'axios';

// may not remove '.js' because of strange module resolution strategy
import config from './config.js';

type RegionInfo = {
  tables: string[];
  regionUrl: string;
};

type Request = {
  url?: string;
  method?: 'GET' | 'POST' | 'PUT' | 'DELETE';
  params?: any;
  data?: any;
  contentType?: 'application/json' | 'text/plain';
};

type SqlResponse = {
  code: number;
  msg: string;
};

class MacroSQLClient {
  private cache?: RegionInfo[];
  private masterUrl!: string;
  private http!: AxiosInstance;

  public static async create(masterUrl: string) {
    const client = new MacroSQLClient(masterUrl);
    await client.loadCache();
    return client;
  }

  private constructor(masterUrl: string) {
    this.masterUrl = masterUrl;
    this.http = Axios.create();
  }

  private async loadCache() {
    try {
      if (fs.existsSync(config.regionCache)) {
        const cache = fs.readFileSync(config.regionCache);
        this.cache = JSON.parse(cache.toString('utf-8'));
      } else {
        this.flushCache();
      }
    } catch (e) {
      this.exceptionHandler(e);
    }
  }

  private async flushCache() {
    try {
      const newCache = await this.request<RegionInfo[]>({
        url: this.masterUrl + '/cache',
      });
      fs.writeFileSync(
        config.regionCache,
        Buffer.from(JSON.stringify(newCache))
      );
      this.cache = newCache;
    } catch (e) {
      this.exceptionHandler(e);
    }
  }

  async queryRegion(tableName: string, sql: string): Promise<SqlResponse> {
    return this._queryRegion(tableName, sql, 3);
  }

  private async _queryRegion(
    tableName: string,
    sql: string,
    flushAndRetryTimes = 3
  ): Promise<SqlResponse> {
    if (flushAndRetryTimes < 0) {
      throw 'Max Retry Times Exceeded';
    }
    try {
      let resp = undefined;
      if (this.cache) {
        const targetRegion = this.cache.find(
          ({ tables }) => tables.indexOf(tableName) !== -1
        );
        if (targetRegion) {
          resp = await this.request<SqlResponse>({
            url: targetRegion.regionUrl,
            method: 'POST',
            data: { command: sql },
          });
        }
      }
      if (!resp) {
        await this.flushCache();
        resp = await this._queryRegion(tableName, sql, flushAndRetryTimes - 1);
      }
      return resp;
    } catch (e) {
      this.exceptionHandler(e);
    }
  }

  async queryMaster(sql: string): Promise<SqlResponse> {
    try {
      return await this.request({
        url: this.masterUrl + '/statement',
        method: 'POST',
        data: { command: sql },
      });
    } catch (e) {
      this.exceptionHandler(e);
    }
  }

  private async request<T>({
    url = this.masterUrl,
    method = 'GET',
    params,
    data,
    contentType = 'application/json',
  }: Request) {
    console.log(url, method, params, data, contentType);
    try {
      const res = await this.http.request<T>({
        url,
        method,
        params,
        data,
        headers: { 'Content-Type': contentType },
      });
      if (res.status.toString().startsWith('2')) {
        const { data } = res;
        return data;
      }
      throw res;
    } catch (e) {
      this.exceptionHandler(e);
    }
  }

  debug() {
    console.log(this.cache);
  }

  private exceptionHandler(e: unknown): never {
    console.log(e);
    process.exit(-1);
  }
}

async function main() {
  const client = await MacroSQLClient.create('localhost:1323');
  client.queryRegion('foo', 'select * from foo;');
}

main();
