# MiniSQL-Client

Cache region's address, send read-only request to region server and write statements to master server.

## Warning

_It is used for course assignment only, you may not use it in production environment~_

## Usage

_Note that all functions implemented in the package are asynchronous~_

To create an instance of the client:

```ts
const client = await MiniSQLClient.create('xxx');
```

Where `xxx` is the master server's address.

To send commands, use

```ts
await client.queryRegion([tableName], [sql]);
```

or

```ts
await client.queryMaster([tableName], [sql]);
```

Note that the first one is for read-only queries, the second one is for write queries.
