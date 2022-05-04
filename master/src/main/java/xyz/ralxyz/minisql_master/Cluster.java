package xyz.ralxyz.minisql_master;

import lombok.extern.slf4j.Slf4j;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.framework.recipes.cache.CuratorCache;
import org.apache.curator.retry.ExponentialBackoffRetry;
import org.apache.curator.test.TestingCluster;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Service;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Arrays;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Slf4j
@Service
public class Cluster {
    public TestingCluster testingCluster;

    public CuratorFramework client;

    public static ExecutorService threadPool = Executors.newFixedThreadPool(2);

    public ConcurrentHashMap<String, String> zkPathMap = new ConcurrentHashMap<>();

    @Autowired
    Config config;

    @Bean
    private void init() throws Exception {
        this.testingCluster = new TestingCluster(3);
        this.testingCluster.start();

        this.client = CuratorFrameworkFactory.builder()
                .connectString(this.testingCluster.getConnectString())
                .retryPolicy(new ExponentialBackoffRetry(1000, 3))
                .build();
        this.client.start();

        this.client.create().creatingParentsIfNeeded().forPath("/db");

        creteCallback();
        startRegions();
    }

    private void creteCallback() {
        var cache = CuratorCache.build(this.client, "/db");
        cache.start();

        cache.listenable().addListener(
                (eventType, oldData, newData) -> {
                    log.info("create node: " + newData.getPath());
                    log.info("create type: " + eventType);
                    log.info("create data: " + new String(newData.getData()));

                    switch (eventType) {
                        case NODE_CREATED, NODE_CHANGED -> zkPathMap.put(newData.getPath(), Arrays.toString(newData.getData()));
                        case NODE_DELETED -> zkPathMap.remove(newData.getPath());
                    }
                },
                threadPool
        );
    }

    private void startRegions() throws Exception {
        var connectStringList = java.util.Arrays.stream(this.testingCluster.getConnectString().split(",")).toList();
        for (int i = 0; i < connectStringList.size(); i++) {
            var builder = new ProcessBuilder();
            builder.command(
                    config.shellPath.get(i),
                    config.zkPathName.get(i),
                    connectStringList.get(i),
                    config.regionPort.get(i)
            );

            log.info("shellPath: " + config.shellPath.get(i));
            log.info("zkPathName: " + config.zkPathName.get(i));
            log.info("zkUrl: " + connectStringList.get(i));
            log.info("regionPort: " + config.regionPort.get(i));

            log.info("start: " + builder.command().stream().reduce((a, b) -> a + " " + b).get());

            var process = builder.start();
        }
    };
}
