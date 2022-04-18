ThisBuild / version := "0.1.0-SNAPSHOT"

ThisBuild / scalaVersion := "3.1.2"

lazy val root = (project in file("."))
  .settings(
    name := "ScalaHelloworld",
    libraryDependencies += "org.apache.curator" % "curator-recipes" % "5.2.1",
    libraryDependencies += "org.apache.curator" % "curator-test" % "5.2.1",
  )
