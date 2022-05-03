import MiniSQLClient from './src/client';

const main = async () => {
  const myClient = await MiniSQLClient.create('localhost:8080');
  myClient.queryRegion('FOO', 'SELECT * FROM FOO WHERE ID < 5;');
};
