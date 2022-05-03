package xyz.ralxyz.minisql_master.model;

import java.util.List;

public record RegionInfo(
        List<String> tables,
        String regionUrl
) {}
