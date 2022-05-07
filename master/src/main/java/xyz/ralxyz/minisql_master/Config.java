package xyz.ralxyz.minisql_master;

import org.springframework.beans.factory.annotation.Value;
import org.springframework.context.annotation.Configuration;

import java.util.List;

@Configuration
public class Config {
    @Value("${minisql.zk_url}")
    public List<String> zkUrl;

    @Value("${minisql.zk_path_name}")
    public List<String> zkPathName;

    @Value("${minisql.region_port}")
    public List<String> regionPort;

    @Value("${minisql.shell_path}")
    public List<String> shellPath;

    @Value("${minisql.region_replica}")
    public int regionReplica;

}
