package xyz.ralxyz.minisql_master;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;
import xyz.ralxyz.minisql_master.model.RegionInfo;
import xyz.ralxyz.minisql_master.model.Statement;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

@Slf4j
@RestController
public class Controller {

    @Autowired
    Cluster cluster;

    @GetMapping("/cache")
    public List<RegionInfo> getCache() {
        var regionInfoList = new ArrayList<RegionInfo>();

        cluster.zkPathMap.forEach((k, v) ->
                regionInfoList.add(new RegionInfo(Arrays.stream(k.split(",")).toList(), v))
        );

        log.info(cluster.testingCluster.getConnectString());
        log.info(regionInfoList.toString());
        // cluster.client.getData().forPath()

        return regionInfoList;
    }

    @PostMapping("/statement")
    public void postStatement(@RequestBody Statement statement) {
        log.info(statement.command());
    }
}
