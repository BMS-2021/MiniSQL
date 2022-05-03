package xyz.ralxyz.minisql_master;

import lombok.extern.slf4j.Slf4j;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.boot.CommandLineRunner;
import org.springframework.boot.SpringApplication;
import org.springframework.boot.autoconfigure.SpringBootApplication;
import org.springframework.context.ApplicationContext;
import org.springframework.context.annotation.Bean;

@Slf4j
@SpringBootApplication
public class MiniSqlMasterApplication {

    @Autowired
    private Config config;

    public static void main(String[] args) {
        SpringApplication.run(MiniSqlMasterApplication.class, args);
    }

    @Bean
    public CommandLineRunner commandLineRunner(ApplicationContext ctx) {
        return args -> {
            for (int i = 0; i < config.zkUrl.size(); i++) {
                var builder = new ProcessBuilder();
                builder.command(
                        config.binPath.get(i),
                        config.zkPathName.get(i),
                        config.zkUrl.get(i),
                        config.regionPort.get(i)
                );

                log.info("zkUrl: " + config.zkUrl.get(i));
                log.info("zkPathName: " + config.zkPathName.get(i));
                log.info("regionPort: " + config.regionPort.get(i));
                log.info("binPath: " + config.binPath.get(i));

                log.info("start: " + builder.command().stream().reduce((a, b) -> a + " " + b).get());

                // builder.start();

                /*
                var process = builder.start();
                var is = process.getInputStream();
                process.waitFor();
                new BufferedReader(new InputStreamReader(is)).lines()
                        .forEach(System.out::println);
                 */
            }
        };
    }

}
