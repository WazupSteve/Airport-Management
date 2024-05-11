# Airport Booking System

The Airport Booking System is a command-line application implemented in C that allows users to manage flight bookings. It provides functionalities for user registration, authentication, flight management, ticket booking, and cancellation. Administrators have additional privileges for managing flights.

## Table of Contents

- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Compilation](#compilation)
  - [Usage](#usage)
- [File Structure](#file-structure)
- [Contributing](#contributing)
- [License](#license)

## Features

- **User Authentication**: Users can register, login, and manage their profiles.
- **Flight Management**: Administrators can add, remove, and view flights.
- **Ticket Booking**: Users can book tickets for available flights.
- **Ticket Cancellation**: Users can cancel their booked tickets.
- **Logging**: System actions are logged in a log file (`system.log`).

## Getting Started

### Prerequisites

To compile and run the Airport Booking System, you need:

- A C compiler (e.g., GCC)
- Terminal or command prompt

### Compilation

Compile the source code using a C compiler. For example, using GCC:

```bash
gcc main.c -o airport_booking_system
```

### Usage

Run the compiled executable:

```bash
./airport_booking_system
```

Follow the on-screen instructions to navigate through the menu and use the functionalities of the system.

## File Structure

- `main.c`: Main source code file containing the implementation of the Airport Booking System.
- `users.txt`: Text file storing user registration details.
- `flights.txt`: Text file storing flight details.
- `bookings.txt`: Text file storing booked tickets.
- `system.log`: Log file storing system actions like user registration, login, ticket booking, etc.

## Contributing

Contributions are welcome! If you'd like to contribute to the project, please follow these steps:

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/YourFeature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin feature/YourFeature`)
5. Open a pull request

Please ensure your pull request adheres to the [code of conduct](CODE_OF_CONDUCT.md).

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
