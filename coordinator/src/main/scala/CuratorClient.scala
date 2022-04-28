import com.google.common.graph.ElementOrder.Type
import org.apache.curator.RetryPolicy
import org.apache.curator.framework.api.CuratorEvent
import org.apache.curator.framework.recipes.cache.*
import org.apache.curator.framework.{CuratorFramework, CuratorFrameworkFactory}
import org.apache.curator.retry.ExponentialBackoffRetry
import org.apache.curator.test.TestingCluster

object CuratorClient:
  private var _client: CuratorFramework = _

  init()

  private def init(): Unit =
    println("Create a testing cluster")
    val cluster = new TestingCluster(3)
    cluster.start()

    val conn = cluster.getConnectString()
    println(s"Connect to $conn")

    val retryPolicy = new ExponentialBackoffRetry(1000, 3)

    println("connect to the testing cluster")
    _client = CuratorFrameworkFactory.builder().connectString(cluster.getConnectString).retryPolicy(retryPolicy).build()


  def Start(): Unit =
    println("Starting CuratorClient")
    _client.start()

  def CreateNode(path: String, data: String): Unit =
    _client.create().creatingParentsIfNeeded().forPath(path, data.getBytes())

  def GetNode(path: String): String =
    new String(_client.getData.forPath(path))

  def SetNode(path: String, data: String): Unit =
    _client.setData().forPath(path, data.getBytes())

  def DeleteNode(path: String): Unit =
    _client.delete().forPath(path)

  def Close(): Unit =
    _client.close()

  def GetClient(): CuratorFramework =
    _client

  def AddListener(path: String): Unit =
    val cache: CuratorCache = CuratorCache.build(_client, path)
    cache.start()

    // add listener

    cache.listenable.addListener((eventType: CuratorCacheListener.Type, oldData: ChildData, newData: ChildData) => {
      println(s"type: $eventType")
      println(s"old data: $oldData")
      println(s"new data: $newData")
    })

