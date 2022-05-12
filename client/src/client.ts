import fs from 'fs';
import Axios from 'axios';
import type { AxiosInstance } from 'axios';

// may not remove '.js' because of strange module resolution strategy
import config from './config';

import { RegionInfo, Request, SqlResponse } from './types';

class MiniSQLClient {
  private cache?: RegionInfo[];
  private masterUrl!: string;
  private http!: AxiosInstance;

  public static async create(masterUrl: string) {
    const client = new MiniSQLClient(masterUrl);
    await client.loadCache();
    return client;
  }

  private constructor(masterUrl: string) {
    this.masterUrl = masterUrl;
    this.http = Axios.create({ proxy: false });
  }

  private async loadCache() {
    try {
      console.log('Loading Cache...');
      if (fs.existsSync(config.regionCache)) {
        const cache = fs.readFileSync(config.regionCache);
        this.cache = JSON.parse(cache.toString('utf-8'));
      } else {
        await this.flushCache();
      }
      console.log('Load cache ok!');
    } catch (e) {
      this.exceptionHandler(e);
    }
  }

  private async flushCache() {
    try {
      console.log('Flushing Cache...');
      const newCache = await this.request<RegionInfo[]>({
        url: this.masterUrl + '/cache',
      });
      fs.writeFileSync(
        config.regionCache,
        Buffer.from(JSON.stringify(newCache))
      );
      this.cache = newCache;
      console.log('Flush cache ok with: ', JSON.stringify(newCache));
    } catch (e) {
      this.exceptionHandler(e);
    }
  }

  async queryRegion(tableName: string, sql: string): Promise<SqlResponse> {
    console.log('Querying To Region Server...');
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
        const targetRegions = this.cache.filter(({ tables }) =>
          tables.find(
            (table) => table.toLocaleLowerCase === tableName.toLocaleLowerCase
          )
        );
        if (targetRegions.length > 0) {
          // load balancing
          const index = Math.floor(Math.random() * targetRegions.length);
          const targetRegion = targetRegions[index];

          console.log(`Select Region ${targetRegion.regionUrl} as target~`);

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

  async queryMaster(tableName: string, sql: string): Promise<SqlResponse> {
    try {
      console.log('Sending request to master...');
      return await this.request({
        url: this.masterUrl + '/statement',
        method: 'POST',
        data: { tableName, command: sql },
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

  private exceptionHandler(e: any): never {
    if (e.response) {
      console.log('AXIOS RESPONSE ERROR');
      console.log('data: ', e.response.data);
      console.log('status: ', e.response.data);
    } else if (e.request) {
      console.log('AXIOS REQUEST ERROR');
      console.log('request: ', e.request);
    } else if (e.message) {
      console.log('AXIOS UNKNOWN ERROR');
      console.log('err msg: ', e.message);
    } else {
      console.log('OTHER ERROR');
      console.log(e);
    }
    process.exit(-1);
  }
}

export default MiniSQLClient;
