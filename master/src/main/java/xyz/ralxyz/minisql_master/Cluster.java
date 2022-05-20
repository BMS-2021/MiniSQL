package xyz.ralxyz.minisql_master;

import lombok.extern.slf4j.Slf4j;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.framework.recipes.cache.CuratorCache;
import org.apache.curator.framework.recipes.cache.CuratorCacheListener;
import org.apache.curator.retry.ExponentialBackoffRetry;
import org.apache.curator.test.TestingCluster;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.context.annotation.Bean;
import org.springframework.stereotype.Service;

import java.io.BufferedReader;
import java.io.File;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.Executors;

@Slf4j
@Service
public class Cluster {
    public TestingCluster testingCluster;

    public CuratorFramework client;

    public static ExecutorService threadPool = Executors.newFixedThreadPool(2);

    public ConcurrentHashMap<String, List<String>> zkPathMap = new ConcurrentHashMap<>();

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
                    if (eventType == CuratorCacheListener.Type.NODE_DELETED) {
                        final var name = Arrays.stream(oldData.getPath().split("/")).reduce((a, b) -> b).get();
                        var connectStringList = java.util.Arrays.stream(this.testingCluster.getConnectString().split(",")).toList();
                        zkPathMap.remove(name);
                        final var i = config.zkPathName.indexOf(name);
                        try {
                            new ProcessBuilder().command(
                                    "./MiniSQL",
                                    config.zkPathName.get(i),
                                    connectStringList.get(i),
                                    config.regionPort.get(i)
                            ).directory(new File(config.shellPath.get(i))).start();
                        } catch (IOException e) {
                            throw new RuntimeException(e);
                        }
                        return;
                    }
                    log.info("create node: " + newData.getPath());
                    log.info("create type: " + eventType);
                    log.info("create data: " + new String(newData.getData()));

                    switch (eventType) {
                        case NODE_CREATED, NODE_CHANGED -> {
                            if (!newData.getPath().equals("/db")) {
                                final var tableListRaw = new String(newData.getData());
                                zkPathMap.put(
                                        Arrays.stream(newData.getPath().split("/")).reduce((a, b) -> b).get(),
                                        tableListRaw.length() == 0 ? new ArrayList<>() : Arrays.stream(tableListRaw.split(",")).toList()
                                );
                            }
                        }
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
                    "./MiniSQL",
                    config.zkPathName.get(i),
                    connectStringList.get(i),
                    config.regionPort.get(i)
            ).directory(new File(config.shellPath.get(i)));

            log.info("shellPath: " + config.shellPath.get(i));
            log.info("zkPathName: " + config.zkPathName.get(i));
            log.info("zkUrl: " + connectStringList.get(i));
            log.info("regionPort: " + config.regionPort.get(i));

            log.info("start: " + builder.command().stream().reduce((a, b) -> a + " " + b).get());

            var process = builder.start();
        }
    };
}
