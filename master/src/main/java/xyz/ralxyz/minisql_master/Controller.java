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

import java.net.SocketException;
import java.util.*;

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
        var isCreate = false;
        var isDrop = false;
        var isExit = false;

        final var commandList = Arrays.stream(statement.command().toLowerCase().split(" +")).toList();

        if (commandList.size() > 0 && commandList.get(0).startsWith("exit")) {
            isExit = true;
        }

        boolean finalIsExit = isExit;
        cluster.zkPathMap.forEach((k, v) -> {
            if (finalIsExit || v.contains(statement.tableName())) {
                regionList.add(k);
            }
        });


        if (commandList.size() >= 2 && commandList.get(0).equals("create") && commandList.get(1).equals("table")) {
            isCreate = true;
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
        } else if (commandList.size() >= 2 && commandList.get(0).equals("drop") && commandList.get(1).equals("table")) {
            isDrop = true;
        }

        SqlResponse resp = null;

        log.info(regionList.toString());

        for (final var region : regionList) {
            final var headers = new HttpHeaders();
            headers.setContentType(MediaType.APPLICATION_JSON);
            final var cmdJsonObject = new JSONObject();
            cmdJsonObject.put("command", statement.command());

            log.info(cmdJsonObject.toString());

            try {
                final var personResultAsJsonStr = new RestTemplate().postForObject(
                        "http://" + getRegionUrl(region),
                        new HttpEntity<>(cmdJsonObject.toString(), headers),
                        SqlResponse.class
                );
                resp = personResultAsJsonStr;
            } catch (Exception e) {
                resp = new SqlResponse(1, "bye!");
            }

            // normal response (0) or exit (1)
            if (resp.code() < 0) {
                break;
            }

            var tableListRaw = new String(this.cluster.client.getData().forPath("/db/" + region));
            if (isCreate) {
                if (tableListRaw.length() > 0) {
                    tableListRaw += ",";
                }
                tableListRaw += statement.tableName();
            } else if (isDrop) {
                var tableList = tableListRaw.split(",");
                // remove tableName from tableList
                tableList = Arrays.stream(tableList).filter(s -> !s.equals(statement.tableName())).toArray(String[]::new);
                tableListRaw = String.join(",", tableList);
            }

            this.cluster.client.setData().forPath("/db/" + region, tableListRaw.getBytes());
        }

        if (resp == null) {
            resp = new SqlResponse(-1, "ERROR: invalid table name");
        }

        return resp;
    }

    private String getRegionUrl(final String pathName) {
        return "localhost:" + config.regionPort.get(config.zkPathName.indexOf(pathName));
    }
}
