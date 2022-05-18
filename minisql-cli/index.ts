import MiniSQLClient from '@enzymii/minisql-client';
import Inquirer from 'inquirer';
import Chalk from 'chalk';

type Answer = {
  sql: string;
  table: string;
  isSelect: boolean;
};

(async () => {
  const client = await MiniSQLClient.create();
  while (true) {
    const { sql, table, isSelect } = await Inquirer.prompt<Answer>([
      {
        type: 'input',
        name: 'sql',
        message: 'Please input your SQL:',
      },
      {
        type: 'input',
        name: 'table',
        message: 'Please input the table name:',
      },
      {
        type: 'confirm',
        name: 'isSelect',
        message: 'Is this a select statement?',
        default: false,
      },
    ]);

    const { code, msg } = isSelect
      ? await client.queryRegion(table, sql)
      : await client.queryMaster(table, sql);

    if (code === 0) {
      console.log(msg);
    } else if (code > 0) {
      console.log(Chalk.cyanBright(msg));
    } else {
      console.log(Chalk.redBright(msg));
    }

    if (sql.toLocaleLowerCase() === 'exit;') {
      break;
    }
  }
})();
