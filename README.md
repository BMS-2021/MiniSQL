 

![MiniSQL](https://raw.githubusercontent.com/RalXYZ/repo-pictures/main/MiniSQL/minisql_overall.png)

# MiniSQL

**Disclaimer: This project is the final project of a course, which can only be considered as a toy. It shows some undergraduates' ideas on how to build a proper database. Suggestions are welcomed.**

MiniSQL is designed to be a distributed relational database system. SQL parsing, execution and storing are implemented in `./db`, we called it a *MiniSQL Region*. Since region is written in compiled languages, it can be compiled into an executable, and run independently. Region can also be linked with a "web service shell" written in golang, which forms a *Region Server*, that is, a region with HTTP service. Master, implemented in `./master`, has the ability to manage multiple region servers, because the state of all nodes are stored in Apache Zookeeper. Sadly, since this project is only a demo written by some undergraduates, It only supports one master, $m$ regions and $n$ replication of data, where $m$ is a arbitrary positive integer, and $n < m$. The system introduced above is called *MiniSQL Cluster*.  
To Interact with the cluster, we implemented a NPM client in `./client`. Since client is just functions that sends and receive messages, to make the whole project usable, we implemented a cli in `./cli`, which relies on the npm client to interact with the cluster.  

The picture above shows the architecture of MiniSQL cluster. To be honest, we didn't implement everything shown on this picture, but surely, you can add some features based on our current implementation to achieve these features without changing the architecture. Some unimplemented features can be implemented only using less than a fifty lines of code. We didn't implement it because as undergraduates, we do not have enough time to implement everything.  
- Figure one shows that many clients can connect to MiniSQL cluster. Although we only implemented a npm client, but implementing clients using other languages is just a matter of time.  
- Figure two shows the architecture inside the cluster, which has been talked about in `./master`.  
- Figure three shows the relationship between master and region. The relationship between the same types of nodes are equivalent. Although we only support one master node currently, with the addition of some locks and interacting with Zookeeper, we surly can support multiple masters nodes. 
- Figure four shows that different nodes can exist on different machines. Although in our current implementation, nodes can only exist on one machine, the only difference between starting a process locally or remotely is whether we need to build a tunnel, for example using *ssh*.  

In each directory, there exists detailed descriptions about the corresponding module.  