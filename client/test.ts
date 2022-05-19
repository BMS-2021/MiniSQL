import axios from 'axios';
import MiniSQLClient from './src/client';

const main = async () => {
  const myClient = await MiniSQLClient.create('http://localhost:8080');
  const req = await myClient.queryRegion('foo', 'SELECT * FROM foo;');
  console.log(req.msg);
};

main();
