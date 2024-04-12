#include "app.hpp"

class App::Impl {
  Config config_;

  //* Thread

  //* Network
  std::unique_ptr<Communication> com;

  //* EMC
  std::shared_ptr<robotics::utils::EMC> emc;
  robotics::node::DigitalOut emc_out;

  //* Components
  //! some

 public:
  Impl(App::Config &config)
      : com(std::make_unique<Communication>(config.com)),
        emc(std::make_shared<robotics::utils::EMC>()),
        emc_out(std::make_shared<robotics::driver::Dout>(PC_1)) {
    auto keep_alive = emc->AddNode();
    this->com->can_.OnKeepAliveLost([keep_alive]() {
      printf("EMC(CAN) setted to %d\n", false);
      keep_alive->SetValue(false);
    });
    this->com->can_.OnKeepAliveRecovered([keep_alive]() {
      printf("EMC(CAN) setted to %d\n", true);
      keep_alive->SetValue(true);
    });
  }

  void MainThread() {
    int i = 0;
    while (1) {
      i++;

      ThisThread::sleep_for(1ms);
    }
  }
  void ReportThread() {
    while (1) {
      com->SendNonReactiveValues();
      com->Report();

      ThisThread::sleep_for(1ms);
    }
  }

  void Init() {
    printf("\e[1;32m-\e[m Init\n");

    printf("\e[1;32m|\e[m \e[32m-\e[m EMC\n");
    emc->output.Link(emc_out);
    emc->Init();

    printf("\e[1;32m|\e[m \e[32m-\e[m COM\n");
    com->Init();
    if (config_.can1_debug) com->AddCAN1Debug();

    printf("\e[1;32m+\e[m   \e[33m+\e[m\n");

    com->SetStatus(robotics::network::DistributedCAN::Statuses::kReady);
  }
};

App::App(Config &config) : impl(new Impl(config)) {}

void App::Init() { this->impl->Init(); }