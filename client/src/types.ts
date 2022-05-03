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

export { RegionInfo, Request, SqlResponse };
