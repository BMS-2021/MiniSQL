package xyz.ralxyz.minisql_master;

import lombok.extern.slf4j.Slf4j;
import org.apache.curator.framework.CuratorFramework;
import org.apache.curator.framework.CuratorFrameworkFactory;
import org.apache.curator.retry.ExponentialBackoffRetry;
import org.apache.curator.test.TestingCluster;
import org.apache.zookeeper.CreateMode;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;

import java.io.IOException;
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
    private Config config;

    public Cluster() throws Exception {
        this.testingCluster = new TestingCluster(3);
        this.testingCluster.start();

        this.client = CuratorFrameworkFactory.builder()
                .connectString(this.testingCluster.getConnectString())
                .retryPolicy(new ExponentialBackoffRetry(1000, 3))
                .build();
        this.client.start();

        creteCallback();
        startRegions();
    }

    private void creteCallback() throws Exception {
        this.client.create().creatingParentsIfNeeded().withMode(CreateMode.EPHEMERAL).inBackground(
                (curatorFramework, curatorEvent) -> {
                    log.info("create node: " + curatorEvent.getPath());
                    log.info("create type: " + curatorEvent.getType());
                    log.info("create data: " + Arrays.toString(curatorEvent.getData()));

                    switch (curatorEvent.getType()) {
                        case CREATE, SET_DATA -> zkPathMap.put(curatorEvent.getPath(), Arrays.toString(curatorEvent.getData()));
                        case DELETE -> zkPathMap.remove(curatorEvent.getPath());
                    }
                },
                threadPool
        ).forPath("/");
    }

    private void startRegions() throws IOException {
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

                /*
                var process = builder.start();
                var is = process.getInputStream();
                process.waitFor();
                new BufferedReader(new InputStreamReader(is)).lines()
                        .forEach(System.out::println);
                 */
        }
    };
}
