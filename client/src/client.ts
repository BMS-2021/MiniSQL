import fs from 'fs';
import Axios from 'axios';
import chalk from 'chalk';
import inquirer from 'inquirer';
import type { AxiosInstance } from 'axios';

// may not remove '.js' because of strange module resolution strategy
import config from './config';

import { RegionInfo, Request, SqlResponse } from './types';

class MiniSQLClient {
  private cache?: RegionInfo[];
  private masterUrl!: string;
  private http!: AxiosInstance;

  public static async create(masterUrl?: string) {
    if (!masterUrl) {
      masterUrl = (
        await inquirer.prompt<{ masterUrl: string }>([
          {
            type: 'input',
            name: 'masterUrl',
            message: 'Please input master url:',
            validate: (input) =>
              /^[a-z0-9.](:\d+)?$/.test(input) || chalk.red('Invalid URL!'),
          },
        ])
      ).masterUrl;
    }
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
      console.log(chalk.cyan('Loading Cache...'));
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
      console.log(chalk.blueBright('Flushing Cache...'));
      const newCache = await this.request<RegionInfo[]>({
        url: this.masterUrl + '/cache',
      });
      fs.writeFileSync(
        config.regionCache,
        Buffer.from(JSON.stringify(newCache))
      );
      this.cache = newCache;
      console.log(
        chalk.green('Flush cache ok with: '),
        JSON.stringify(newCache)
      );
    } catch (e) {
      this.exceptionHandler(e);
    }
  }

  async queryRegion(tableName: string, sql: string): Promise<SqlResponse> {
    console.log(chalk.magenta('Querying To Region Server...'));
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
            (table) =>
              table.toLocaleLowerCase() === tableName.toLocaleLowerCase()
          )
        );
        if (targetRegions.length > 0) {
          // load balancing
          const index = Math.floor(Math.random() * targetRegions.length);
          const targetRegion = targetRegions[index];

          console.log(
            chalk.yellow(`Select Region ${targetRegion.regionUrl} as target~`)
          );

          resp = await this.request<SqlResponse>({
            url: 'http://' + targetRegion.regionUrl,
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
      console.log(chalk.magenta('Sending request to master...'));
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
      console.log(chalk.redBright.bold('AXIOS RESPONSE ERROR'));
      console.log(chalk.red('data: '), e.response.data);
      console.log(chalk.red('status: '), e.response.data);
    } else if (e.request) {
      console.log(chalk.redBright.bold('AXIOS REQUEST ERROR'));
      console.log(chalk.red('request: '), e.request);
    } else if (e.message) {
      console.log(chalk.redBright.bold('AXIOS UNKNOWN ERROR'));
      console.log(chalk.red('err msg: '), e.message);
    } else {
      console.log(chalk.redBright.bold('OTHER ERROR'));
      console.log(chalk.red(e));
    }
    process.exit(-1);
  }
}

export default MiniSQLClient;
