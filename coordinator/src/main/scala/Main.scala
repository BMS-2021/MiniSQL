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

    CuratorClient.AddListener("/test")

    CuratorClient.CreateNode("/test/ababa", "ababa")

    Thread.sleep(1000)

    CuratorClient.SetNode("/test/ababa", "acacia")

    Thread.sleep(1000)

    CuratorClient.DeleteNode("/test/ababa")

    Thread.sleep(1000)

    CuratorClient.Close()
