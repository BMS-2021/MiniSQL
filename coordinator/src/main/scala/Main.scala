import org.apache.curator.test.{TestingCluster, TestingZooKeeperServer}
import org.apache.curator.framework.{CuratorFrameworkFactory, CuratorFramework}
import org.apache.curator.retry.ExponentialBackoffRetry

import scala.jdk.CollectionConverters.*

object Main:

  def createNode(client: CuratorFramework, path: String, value: String): Unit =
    val node = client.create().creatingParentsIfNeeded().forPath(path, value.getBytes)
    println(s"Created node: $node")


  def main(args: Array[String]): Unit =
    // create a curator client
    CuratorClient.Start()
    CuratorClient.Close()
