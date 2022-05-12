import axios from 'axios';
import MiniSQLClient from './src/client';

const main = async () => {
  const myClient = await MiniSQLClient.create('http://localhost:8080');
  // const req = myClient.queryRegion('FOO', 'SELECT * FROM FOO WHERE ID < 5;');
  const req = await myClient.queryMaster('foo', 'CREATE TABLE foo (bar INT);');
  console.log(req);
};

main();
