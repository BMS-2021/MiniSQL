package xyz.ralxyz.minisql_master;

import com.alibaba.fastjson2.JSONObject;
import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpEntity;
import org.springframework.http.HttpHeaders;
import org.springframework.http.MediaType;
import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;
import org.springframework.web.client.RestTemplate;
import xyz.ralxyz.minisql_master.model.RegionInfo;
import xyz.ralxyz.minisql_master.model.SqlResponse;
import xyz.ralxyz.minisql_master.model.Statement;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.Random;

@Slf4j
@RestController
public class Controller {

    Random rand = new Random();

    @Autowired
    Cluster cluster;

    @Autowired
    Config config;

    @GetMapping("/cache")
    public List<RegionInfo> getCache() {
        final var regionInfoList = new ArrayList<RegionInfo>();

        log.info(cluster.zkPathMap.toString());

        cluster.zkPathMap.forEach((k, v) ->
            regionInfoList.add(
                    new RegionInfo(v, getRegionUrl(k))
            )
        );

        log.info(cluster.testingCluster.getConnectString());
        log.info(regionInfoList.toString());

        return regionInfoList;
    }

    @PostMapping("/statement")
    public SqlResponse postStatement(@RequestBody final Statement statement) throws Exception {
        final var regionList = new ArrayList<String>();
        cluster.zkPathMap.forEach((k, v) -> {
            if (v.contains(statement.tableName())) {
                regionList.add(k);
            }
        });

        final var commandList = Arrays.stream(statement.command().toLowerCase().split(" +")).toList();
        if (commandList.size() >= 2 && commandList.get(0).equals("create") && commandList.get(1).equals("table")) {
            if (regionList.size() != 0) {
                return new SqlResponse(-1, "ERROR 200 from <api>: table '" + statement.tableName() + "' exists");
            } else {
                // rand regions, add them to regionUrlList\
                while (regionList.size() < config.regionReplica) {
                    final var randIdx = rand.nextInt(cluster.zkPathMap.size());
                    final var region = (String)cluster.zkPathMap.keySet().toArray()[randIdx];
                    if (!regionList.contains(region)) {
                        regionList.add(region);
                    }
                }
            }
        }

        SqlResponse resp = null;

        log.info(regionList.toString());

        for (final var region : regionList) {
            final var headers = new HttpHeaders();
            headers.setContentType(MediaType.APPLICATION_JSON);
            final var cmdJsonObject = new JSONObject();
            cmdJsonObject.put("command", statement.command());

            log.info(cmdJsonObject.toString());

            final var personResultAsJsonStr = new RestTemplate().postForObject(
                    "http://" + getRegionUrl(region),
                    new HttpEntity<>(cmdJsonObject.toString(), headers),
                    SqlResponse.class
            );
            resp = personResultAsJsonStr;

            if (resp.code() != 0) {
                break;
            }

            var tableListRaw = new String(this.cluster.client.getData().forPath("/db/" + region));
            if (tableListRaw.length() > 0) {
                tableListRaw += ",";
            }
            tableListRaw += statement.tableName();

            this.cluster.client.setData().forPath("/db/" + region, tableListRaw.getBytes());
        }

        return resp;
    }

    private String getRegionUrl(final String pathName) {
        return "localhost:" + config.regionPort.get(config.zkPathName.indexOf(pathName));
    }
}
