# MiniSQL Master

**Disclaimer: This project is the final project of a course, which can only be considered as a toy. It shows some undergraduates' ideas on how to build a proper database. Suggestions are welcomed.** 

MiniSQL master manages all region servers. It starts region servers, keep track on how tables distribute on different regions, load balancing request and so on.  

<div align="center">
  <img src="https://raw.githubusercontent.com/RalXYZ/repo-pictures/main/MiniSQL/minisql_master.jpg" style="width: 70%;"/>
</div>


There are three services in master.  

*Controller Service* is responsible for receiving and pre-processing HTTP requests, of which there are two types.  
- The first one is `GET` cache request, where the client gets the cache information from the master. Since the master will synchronize the region and table correspondence in real time via a callback function, this request is actually taking the mapping table.  
- The second type of request is `POST` statement request. The scenario is that the client sends a request to the master with side effects. As the request body contains table information, the Master master tries to find which region it should hit with the request, again by looking up the mapping table. The structure of the region response is described in `db/go-svc`.  

*Cluster Service* is responsible for region start/stop, disaster recovery and callback function registration. In short, everything related to cluster management is coded here. In a testing environment, this service is also responsible for starting a Zookeeper cluster; regardless of the environment, a Zookeeper Curator Client is always maintained here, which pulls up the region by constructing and executing command-line instructions when the cluster is started or a disaster recovery takes place. On the region side, the master monitors the state of the region by registering a callback function with Curator. The callback function monitors the directory for updates. As soon as an update is made to the Zookeeper directory (typically a new table), the master synchronises the update to the data structure of the region where the table is stored in its own memory.  

*Config Service* is responsible for loading configuration files. Loading configuration files is a native feature of the Spring framework that allows configuration information to be stored in specified class member variables. The configuration file we store includes four fields `zkUrl`, `zkPathName`, `regionPort`, `shellPath`, which are metadata that related to all regions and Zookeeper.  
