# Windows Filter Driver for Performance Troubleshooting Training

## Overview

This project hosts a simple Windows filter driver designed as a training tool for engineers to learn troubleshooting performance issues related to filter drivers in Windows. The driver introduces a configurable random delay to read operations, simulating performance bottlenecks.

## Purpose

The main goal is to provide a hands-on learning experience for engineers and IT professionals. Through this driver, users will gain practical skills in diagnosing and resolving performance problems in Windows environments that are often challenging to understand without real-world examples.

## Features

- **Configurable Delay:** Simulates performance issues by introducing a random delay to read operations.
- **Educational Tool:** Enhances understanding of filter drivers' impact on system performance.
- **Troubleshooting Practice:** Offers a direct way to practice performance troubleshooting techniques.

## Getting Started

### Prerequisites

- Windows Driver Kit (WDK)
- Visual Studio

### Installation

1. Clone this repository.
2. Open the solution in Visual Studio and build the driver.
3. Use the Device Manager or `sc` command line tool to install the driver.
4. Optional Installation script is provided to roll the driver on dev/test debug machine.

**Important:** Test this driver in a non-production environment as it WILL affect system stability.

## Usage

After installation, the driver will start affecting read operations by introducing delays. This provides a scenario for engineers to practice identifying and troubleshooting the source of performance degradation.

## Disclaimer

This driver is for educational purposes only. It's not meant for production use. I assume no responsibility for any system instability or data loss.
