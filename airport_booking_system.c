#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLIGHTS 6
#define SEATS_PER_FLIGHT 60

typedef struct {
    char username[30];
    char password[30];
    char email[50];
    int isAdmin;
} User;

typedef struct {
    char flight_number[10];
    char seats[SEATS_PER_FLIGHT];
    int isActive;
} Flight;

typedef struct Booking {
    char flight_number[10];
    int seat_number;
    struct Booking *next;
} Booking;

void logAction(const char *action) {
    FILE *logFile = fopen("system.log", "a");
    if (logFile) {
        fprintf(logFile, "%s\n", action);
        fclose(logFile);
    }
}

void registerUser() {
    FILE *file = fopen("users.txt", "a+");
    if (!file) {
        printf("Unable to open user file.\n");
        return;
    }

    User newUser;
    printf("Enter username: ");
    scanf("%s", newUser.username);
    printf("Enter password: ");
    scanf("%s", newUser.password);
    printf("Enter email: ");
    scanf("%s", newUser.email);
    newUser.isAdmin = 0;

    fprintf(file, "%s %s %s %d\n", newUser.username, newUser.password, newUser.email, newUser.isAdmin);
    fclose(file);
    printf("User registered successfully!\n");
    logAction("User registered");
}

User login() {
    char username[30], password[30];
    printf("Enter username: ");
    scanf("%s", username);
    printf("Enter password: ");
    scanf("%s", password);

    FILE *file = fopen("users.txt", "r");
    if (!file) {
        printf("User data file missing.\n");
        exit(1);
    }

    User user;
    while (fscanf(file, "%s %s %s %d", user.username, user.password, user.email, &user.isAdmin) == 4) {
        if (strcmp(username, user.username) == 0 && strcmp(password, user.password) == 0) {
            fclose(file);
            logAction("User logged in");
            return user;
        }
    }
    fclose(file);
    printf("Invalid credentials, try again.\n");
    exit(1);
}

void saveFlights(Flight flights[], int numFlights) {
    FILE *file = fopen("flights.txt", "w");
    for (int i = 0; i < numFlights; i++) {
        fprintf(file, "%s %s %d\n", flights[i].flight_number, flights[i].seats, flights[i].isActive);
    }
    fclose(file);
}

void initializeFlights(Flight flights[], int numFlights) {
    FILE *file = fopen("flights.txt", "r");
    if (file) {
        for (int i = 0; i < numFlights && !feof(file); i++) {
            fscanf(file, "%s %s %d\n", flights[i].flight_number, flights[i].seats, &flights[i].isActive);
        }
        fclose(file);
    } else {
        for (int i = 0; i < numFlights; i++) {
            sprintf(flights[i].flight_number, "FL%03d", i + 1);
            memset(flights[i].seats, 'O', SEATS_PER_FLIGHT);
            flights[i].isActive = 1;
        }
        saveFlights(flights, numFlights);
    }
}

void displayFlights(const Flight flights[], int numFlights) {
    printf("Available flights and seats:\n");
    for (int i = 0; i < numFlights; i++) {
        if (flights[i].isActive) {
            printf("%s: \n", flights[i].flight_number);
            for (int j = 0; j < SEATS_PER_FLIGHT; j++) {
                printf("%c", flights[i].seats[j] == 'O' ? '.' : 'X');
                if ((j + 1) % 10 == 0) printf("\n");
            }
            printf("\n");
        }
    }
}

int bookTicket(Flight flights[], int numFlights, Booking **bookings) {
    char flight_num[10];
    int seat_num;
    printf("Enter flight number: ");
    scanf("%s", flight_num);
    printf("Enter seat number: ");
    scanf("%d", &seat_num);

    if (seat_num < 1 || seat_num > SEATS_PER_FLIGHT) {
        printf("Invalid seat number!\n");
        return -1;
    }

    for (int i = 0; i < numFlights; i++) {
        if (strcmp(flights[i].flight_number, flight_num) == 0 && flights[i].isActive) {
            if (flights[i].seats[seat_num - 1] == 'O') {
                flights[i].seats[seat_num - 1] = 'X';

                Booking *new_booking = malloc(sizeof(Booking));
                if (!new_booking) {
                    printf("Memory allocation failed!\n");
                    return -1;
                }
                strcpy(new_booking->flight_number, flight_num);
                new_booking->seat_number = seat_num;
                new_booking->next = *bookings;
                *bookings = new_booking;

                logAction("Ticket booked");
                saveFlights(flights, numFlights); // Update the seat status in the file.
                return 0;
            } else {
                printf("This seat is already booked.\n");
                return -1;
            }
        }
    }
    printf("Flight number is incorrect or flight is not active.\n");
    return -1;
}

int cancelTicket(Flight flights[], int numFlights, Booking **bookings) {
    char flight_num[10];
    int seat_num;
    printf("Enter flight number: ");
    scanf("%s", flight_num);
    printf("Enter seat number: ");
    scanf("%d", &seat_num);

    Booking **current = bookings;
    while (*current) {
        if (strcmp((*current)->flight_number, flight_num) == 0 && (*current)->seat_number == seat_num) {
            for (int i = 0; i < numFlights; i++) {
                if (strcmp(flights[i].flight_number, flight_num) == 0 && flights[i].isActive) {
                    if (flights[i].seats[seat_num - 1] == 'X') {
                        flights[i].seats[seat_num - 1] = 'O';

                        Booking *to_delete = *current;
                        *current = (*current)->next;
                        free(to_delete);

                        logAction("Ticket canceled");
                        saveFlights(flights, numFlights); // Update the seat status in the file.
                        return 0;
                    }
                }
            }
        }
        current = &(*current)->next;
    }
    printf("No such booking found.\n");
    return -1;
}

void freeBookings(Booking *bookings) {
    while (bookings) {
        Booking *next = bookings->next;
        free(bookings);
        bookings = next;
    }
}

void loadBookingsFromTextFile(Booking **bookings) {
    FILE *file = fopen("bookings.txt", "r");
    if (!file) {
        printf("No existing bookings to load.\n");
        return;
    }

    freeBookings(*bookings);
    *bookings = NULL;

    char buffer[100];
    char flight_number[10];
    int seat_number;
    int count = 0;

    while (fgets(buffer, sizeof(buffer), file)) {
        if (sscanf(buffer, "Flight %s Seat %d", flight_number, &seat_number) == 2) {
            Booking *new_booking = malloc(sizeof(Booking));
            if (!new_booking) {
                printf("Memory allocation failed while loading bookings.\n");
                fclose(file);
                return;
            }
            strcpy(new_booking->flight_number, flight_number);
            new_booking->seat_number = seat_number;
            new_booking->next = *bookings;
            *bookings = new_booking;
            count++;
        }
    }
    printf("Loaded %d bookings.\n", count);
    fclose(file);
}

void saveBookingsToTextFile(Booking *bookings) {
    if (!bookings) {
        printf("No bookings to save, file unchanged.\n");
        return;
    }

    FILE *file = fopen("bookings.txt", "w");
    if (!file) {
        printf("Failed to open file for saving.\n");
        return;
    }

    Booking *current = bookings;
    int count = 0;
    while (current) {
        fprintf(file, "Flight %s Seat %d\n", current->flight_number, current->seat_number);
        current = current->next;
        count++;
    }
    printf("Saved %d bookings to file.\n", count);
    fclose(file);
}

void viewUserProfile(User user) {
    printf("Username: %s\n", user.username);
    printf("Email: %s\n", user.email);
    printf("Role: %s\n", user.isAdmin ? "Administrator" : "Regular user");
}

void modifyUserDetails(User *user) {
    char newEmail[50];
    printf("Enter new email: ");
    scanf("%s", newEmail);
    strcpy(user->email, newEmail);

    FILE *file = fopen("users.txt", "r+");
    if (!file) {
        printf("Failed to open users file for update.\n");
        return;
    }

    User tempUser;
    int found = 0;
    while (fscanf(file, "%s %s %s %d", tempUser.username, tempUser.password, tempUser.email, &tempUser.isAdmin) == 4) {
        if (strcmp(tempUser.username, user->username) == 0) {
            found = 1;
            break;
        }
    }

    if (found) {
        fseek(file, -sizeof(User), SEEK_CUR);
        fprintf(file, "%s %s %s %d\n", tempUser.username, tempUser.password, newEmail, tempUser.isAdmin);
    }
    fclose(file);
}

void addFlight(Flight flights[], int *numFlights) {
    if (*numFlights < MAX_FLIGHTS) {
        printf("Enter flight number: ");
        scanf("%s", flights[*numFlights].flight_number);

        // Assuming seats are initially all available.
        memset(flights[*numFlights].seats, 'O', SEATS_PER_FLIGHT);
        flights[*numFlights].isActive = 1;

        (*numFlights)++;
        printf("Flight added successfully!\n");
    } else {
        // Check if there's an inactive flight to replace
        int foundInactive = -1;
        for (int i = 0; i < MAX_FLIGHTS; i++) {
            if (!flights[i].isActive) {
                foundInactive = i;
                break;
            }
        }
        if (foundInactive != -1) {
            printf("Enter flight number: ");
            scanf("%s", flights[foundInactive].flight_number);
            memset(flights[foundInactive].seats, 'O', SEATS_PER_FLIGHT);
            flights[foundInactive].isActive = 1;
            printf("Flight added successfully!\n");
        } else {
            printf("Cannot add more flights, maximum limit reached.\n");
        }
    }
}

void removeFlight(Flight flights[], int *numFlights) {
    char flight_num[10];
    printf("Enter flight number to remove: ");
    scanf("%s", flight_num);

    int found = 0;
    for (int i = 0; i < *numFlights; i++) {
        if (strcmp(flights[i].flight_number, flight_num) == 0 && flights[i].isActive) {
            // Deactivate the flight.
            flights[i].isActive = 0;
            found = 1;
            printf("Flight %s removed successfully!\n", flight_num);
            break;
        }
    }

    if (!found) {
        printf("Flight not found or already inactive.\n");
    }
}

void adminMenu(Flight flights[], int numFlights, Booking **bookings) {
    int choice;
    do {
        printf("\n*** Admin Menu ***\n");
        printf("1. Add Flight\n");
        printf("2. Remove Flight\n");
        printf("3. List All Flights\n");
        printf("4. View All Bookings\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                addFlight(flights, &numFlights);
                break;
            case 2:
                removeFlight(flights, &numFlights);
                break;
            case 3:
                displayFlights(flights, numFlights);
                break;
            case 4:
                printf("Current Bookings:\n");
                if (!*bookings) {
                    printf("No bookings available.\n");
                } else {
                    for (Booking *b = *bookings; b != NULL; b = b->next) {
                        printf("Flight %s, Seat %d\n", b->flight_number, b->seat_number);
                    }
                }
                break;
            case 5:
                printf("Exiting admin menu.\n");
                break;
            default:
                printf("Invalid choice. Please choose again.\n");
        }
    } while (choice != 5);
}

int main() {
    User currentUser;
    printf("1. Login\n0. Register\nChoose an option: ");
    int initChoice;
    scanf("%d", &initChoice);
    if (initChoice == 0) {
        registerUser();
    }
    currentUser = login();

    Flight flights[MAX_FLIGHTS];
    Booking *bookings = NULL;
    int numFlights = MAX_FLIGHTS; // Initialize with max flights

    initializeFlights(flights, MAX_FLIGHTS);
    loadBookingsFromTextFile(&bookings);

    if (currentUser.isAdmin) {
        adminMenu(flights, numFlights, &bookings);
    } else {
        int choice;
        do {
            printf("\n*** Airport Booking System ***\n");
            printf("1. Display Flights\n");
            printf("2. Book Ticket\n");
            printf("3. Cancel Ticket\n");
            printf("4. View User Profile\n");
            printf("5. Modify User Details\n");
            printf("6. Exit\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            switch (choice) {
                case 1:
                    displayFlights(flights, MAX_FLIGHTS);
                    break;
                case 2:
                    if (bookTicket(flights, MAX_FLIGHTS, &bookings) == 0) {
                        printf("Ticket booked successfully!\n");
                    } else {
                        printf("Booking failed. Please try again.\n");
                    }
                    break;
                case 3:
                    if (cancelTicket(flights, MAX_FLIGHTS, &bookings) == 0) {
                        printf("Ticket cancelled successfully!\n");
                    } else {
                        printf("Cancellation failed. Please try again.\n");
                    }
                    break;
                case 4:
                    viewUserProfile(currentUser);
                    break;
                case 5:
                    modifyUserDetails(&currentUser);
                    printf("User details updated successfully.\n");
                    break;
                case 6:
                    printf("Exiting system.\n");
                    saveBookingsToTextFile(bookings);
                    freeBookings(bookings);
                    saveFlights(flights, MAX_FLIGHTS); // Save flights on exit.
                    exit(0);
                default:
                    printf("Invalid choice. Please choose again.\n");
            }
        } while (choice != 6);
    }

    return 0;
}
