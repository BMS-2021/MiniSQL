package xyz.ralxyz.minisql_master;

import org.springframework.web.bind.annotation.GetMapping;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;
import xyz.ralxyz.minisql_master.model.RegionInfo;
import xyz.ralxyz.minisql_master.model.Statement;

import java.util.ArrayList;
import java.util.List;

@RestController
public class Controller {

    @GetMapping("/cache")
    public List<RegionInfo> getCache() {
        var foo = new ArrayList<String>();
        foo.add("foo");
        foo.add("bar");

        var regionInfo = new RegionInfo(foo, "buz");
        var regionInfoList = new ArrayList<RegionInfo>();
        regionInfoList.add(regionInfo);

        return regionInfoList;
    }

    @PostMapping("/statement")
    public void postStatement(@RequestBody Statement statement) {
        System.out.println(statement.command());
    }
}
