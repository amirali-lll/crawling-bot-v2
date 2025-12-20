# Crawling Bot V2 - AI Learning Platform

[![PlatformIO](https://img.shields.io/badge/PlatformIO-ESP32-orange.svg)](https://platformio.org/)
[![IUST](https://img.shields.io/badge/IUST-AI%20Course-blue.svg)](https://iust.ac.ir)
[![VILS Lab](https://img.shields.io/badge/VILS-Lab%20Course-blue.svg)](https://vils-lab.github.io)
[![License](https://img.shields.io/badge/License-MIT-blue.svg)](LICENSE)

> 🎓 **An intelligent crawling robot for IUST University's AI course**  
> Students implement Reinforcement Learning algorithms to teach the robot autonomous movement.

**📖 [مستندات فارسی (Persian Documentation)](README.fa.md)**

![Crawling Bot V2](docs/img/robot-overview.jpg)

## Overview

A hands-on robotics platform where AI students apply **Reinforcement Learning** theory to real hardware. The robot features ESP32 microcontroller, 9-DOF motion sensors, servo actuators, and a modular software architecture designed for easy RL implementation.

**Key Features:**

- 🧠 Ready-to-code RL template in `lib/Training/`
- 🎮 Full sensor suite for state observation (9-DOF AHRS)
- 🔧 Two servo motors for action execution
- 📡 WiFi + OTA for wireless development
- 🖥️ OLED display for real-time feedback

**What Makes It Special:**

- Bridge theory to practice with real hardware
- Modular libraries handle hardware complexity
- Focus on RL algorithm implementation
- Quick iteration with OTA updates

## For Students

### Your 3-Phase Project Journey

**Phase 1: Hello Hardware** (Week 1)

- Upload basic code and test all components
- Make robot display "Hello World"
- Implement simple "bye-bye" wave movement
- ✅ Verify hardware works correctly

**Phase 2: Build Your RL Algorithm** (Weeks 2)

- Design and code your RL algorithm (Q-Learning, SARSA, DQN, etc.)
- Test algorithm logic separately
- Define reward functions and state/action spaces
- ✅ Algorithm ready for integration

**Phase 3: Train Your Robot** (Weeks 3-4)

- Integrate RL algorithm with robot hardware
- Use sensor data for state observation
- Control servos based on learned policy
- ✅ Autonomous crawling achieved!

### What You'll Work With

**Sensors** (for state observation):

- 9-DOF motion sensor (acceleration, rotation, orientation)
- Real-time position and movement tracking

**Actuators** (for actions):

- 2 servo motors for leg control
- Configurable speed and smooth movement

**Your Code Goes Here:** `lib/Training/Training.cpp` - Ready-to-fill template for your RL implementation!

> 📖 **See [EXAMPLES.md](docs/EXAMPLES.md) for RL code samples and [PROJECT_SUMMARY.md](docs/PROJECT_SUMMARY.md) for technical details**

## Getting Started

### Quick Setup (5 Minutes)

1. **Install Tools**

   - Install [VSCode](https://code.visualstudio.com/)
   - Install [PlatformIO extension](https://platformio.org/install/ide?install=vscode)

2. **Clone & Build**

   ```bash
   git clone https://github.com/amirali-lll/crawling-bot-v2.git
   cd crawling-bot-v2
   pio run --target upload
   ```

3. **Configure Robot**

* Turn on the robot using the **On/Off switch located underneath the robot body**.
* The robot will create a Wi-Fi access point named **ESP32-AP-{number}** (where `{number}` is your robot’s assigned ID).
  Connect to the robot’s Wi-Fi network and enter **a number from 1 to 8 (12345678)** in the password field.
* After your device is connected to the robot, **check again whether the previous upload attempt** using the command

  ```bash
  pio run --target upload
  ```

  was successful. If the upload did not complete successfully, **run the same command again** to program the robot.
* If you have followed all the steps and see that the system is uploading and the upload finishes successfully, **congratulations! 🎉**
  This means you are now connected to the robot and can upload and run the code you have written on it. 🙂
  

4. **Start Coding**
   - Open `lib/Training/Training.cpp`
   - Implement your RL algorithm
   - Upload via OTA: Connect to robot WiFi → `pio run --target upload`

> 📖 **Detailed setup guide:** [QUICKSTART.md](docs/QUICKSTART.md) | [BUILD.md](docs/BUILD.md)

## Documentation

| Document                                         | What's Inside                         |
| ------------------------------------------------ | ------------------------------------- |
| 📖 [QUICKSTART.md](docs/QUICKSTART.md)           | 5-minute setup guide                  |
| 🔧 [BUILD.md](docs/BUILD.md)                     | Detailed build & deployment           |
| 💻 [EXAMPLES.md](docs/EXAMPLES.md)               | RL code examples & snippets           |
| 📊 [PROJECT_SUMMARY.md](docs/PROJECT_SUMMARY.md) | Full technical documentation          |
| 🔄 [MIGRATION.md](docs/MIGRATION.md)             | Arduino to PlatformIO migration notes |

## Hardware & Architecture

**Hardware:** ESP32-S3, MPU9250 (9-DOF), 2x Servos, OLED Display  
**Software:** 5 modular libraries (Display, AHRS, ServoControl, Network, Training)  
**Power:** 12V adapter

> 📖 **Full specs:** See [PROJECT_SUMMARY.md](docs/PROJECT_SUMMARY.md#hardware) and [PROJECT_SUMMARY.md](docs/PROJECT_SUMMARY.md#architecture)

## Support & Contributing

- **Questions?** Check [docs/](docs/) folder or open an [issue](https://github.com/amirali-lll/crawling-bot-v2/issues)
- **Contributing:** PRs welcome! Focus on RL implementations, examples, and documentation
- **Course:** IUST University - AI Course

---

**🎓 Make Your Robot Crawl with AI! 🤖**

_Designed for IUST AI students | MIT License | Built with PlatformIO_
