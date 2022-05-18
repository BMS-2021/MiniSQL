import axios from 'axios';
import MiniSQLClient from './src/client';

const main = async () => {
  const myClient = await MiniSQLClient.create('http://localhost:8080');
  const req = await myClient.queryMaster('fooy', 'CREATE TABLE fooy (dd INT);');
  console.log(req.msg);
  await myClient.queryMaster('qwq', 'EXIT;');
  const req2 = await myClient.queryMaster('fooy', 'INSERT INTO fooy VALUES (7);');
  console.log(req2);
  await myClient.queryMaster('qwq', 'EXIT;');
  const req3 = await myClient.queryRegion('fooy', 'SELECT * FROM fooy WHERE dd > 6;');
  console.log(req3.msg);
};

main();
