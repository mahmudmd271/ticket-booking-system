//reg add HKCU\Console /v VirtualTerminalLevel /t REG_DWORD /d 1
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <conio.h>

#define MAX_TICKETS 100 // Maximum number of tickets
#define MAX_NAME_LEN 50 // Maximum length of passenger name
#define MAX_TICKETS_PER_PERSON 3 // Maximum tickets a person can book
#define ADMIN_PASSWORD "0000" // Admin panel password
#define TICKET_PRICE 200 // Price per ticket

// Color Functions
void reset()
{
    printf("\033[0m");
}
void yellow()
{
    printf("\033[0;33m");
}
void purple()
{
    printf("\033[0;35m");
}
void red()
{
    printf("\033[0;31m");
}
void green()
{
    printf("\033[0;32m");
}
void cyan()
{
    printf("\033[0;36m");
}

// Structures for Tickets, Queue, Stack, and Linked List
struct Ticket
{
    int ticket_id;
    char passenger_name[MAX_NAME_LEN];
    int seat_number;
};

struct Ticket tickets[MAX_TICKETS];
int ticket_count = 0;

// Queue Structure
struct TicketQueue
{
    struct Ticket data[MAX_TICKETS];
    int front;
    int rear;
} queue = { .front = -1, .rear = -1 };

// Stack Structure
struct TicketStack
{
    struct Ticket data[MAX_TICKETS];
    int top;
} stack = { .top = -1 };

// Linked List for History
struct HistoryNode
{
    char action[100];
    struct HistoryNode* next;
} *history_head = NULL;

// All functions starts from here
// Queue Functions
void enqueueTicket(struct Ticket ticket)
{
    if ((queue.rear + 1) % MAX_TICKETS == queue.front)
    {
        red();
        printf("Tickets not Available.\n"); //Queue overflow
        reset();

    }
    if (queue.front == -1) queue.front = 0;
    queue.rear = (queue.rear + 1) % MAX_TICKETS;
    queue.data[queue.rear] = ticket;
}

// Stack Functions
void pushToStack(struct Ticket ticket)
{
    if (stack.top == MAX_TICKETS - 1)
    {
        red();
        printf("Please wait until anyone cancel a ticket.\nHave a nice day!");//Stack overflow
        reset();
        getch();
        exit(0);


    }
    stack.data[++stack.top] = ticket;
}

// Linked List Functions
void addToHistory(const char* action)
{
    struct HistoryNode* new_node = (struct HistoryNode*)malloc(sizeof(struct HistoryNode));
    strncpy(new_node->action, action, 99);
    new_node->action[99] = '\0';
    new_node->next = history_head;
    history_head = new_node;
}

void displayHistory()
{
    struct HistoryNode* current = history_head;
    if (!current)
    {
        red();
        printf("No actions in history.\n");
        reset();
        return;
    }
    purple();
    printf("\nAction History:\n");
    reset();
    while (current)
    {
        printf("%s\n", current->action);
        current = current->next;
    }
}

// Ticket Booking Functions
int isSeatAvailable(int seat_number)
{
    for (int i = 0; i < ticket_count; i++)
    {
        if (tickets[i].seat_number == seat_number) return 0;
    }
    return 1;
}

int countTicketsByPerson(const char* name)
{
    int count = 0;
    for (int i = 0; i < ticket_count; i++)
    {
        if (strcmp(tickets[i].passenger_name, name) == 0) count++;
    }
    return count;
}

void ticket_price(int total_tickets)
{
    int cost = TICKET_PRICE * total_tickets;
    printf("\nTotal cost: %d Taka\nPlease insert money: ", cost);
    int payment;
    scanf("%d", &payment);
    while (payment < cost)
    {
        red();
        printf("Insufficient amount! Add more money: ");
        reset();
        int extra;
        scanf("%d", &extra);
        payment += extra;
    }
    green();
    printf("Payment successful!!\nReturned amount: %d Taka.\n", payment - cost);
    reset();
}

void addTicket()
{
    if (ticket_count >= MAX_TICKETS)
    {
        red();
        printf("All tickets are booked.\n");
        reset();
        return;
    }

    char name[MAX_NAME_LEN];
    int num_tickets;

    printf("\nEnter passenger name: ");
    scanf("%s", name);
    cyan();
    printf ("\nThe price of each ticket is 200 taka only\n\n");
    reset();
    int current_tickets = countTicketsByPerson(name);

    if (current_tickets >= MAX_TICKETS_PER_PERSON)
    {
        red();
        printf("You can only book up to %d tickets.\n", MAX_TICKETS_PER_PERSON);
        reset();
        return;
    }

    printf("How many tickets (1-%d)? ", MAX_TICKETS_PER_PERSON - current_tickets);
    scanf("%d", &num_tickets);

    if (num_tickets < 1 || num_tickets > (MAX_TICKETS_PER_PERSON - current_tickets))
    {
        red();
        printf("Invalid number of tickets.\n");
        reset();
        return;
    }

    for (int i = 0; i < num_tickets; i++)
    {
        int seat_number;
        printf("Enter seat number for ticket %d (1-99): ", i + 1);
        scanf("%d", &seat_number);
        if(seat_number <=0 || seat_number >=100)
        {
            red();
            printf ("Invalid seat number!!");
            return;
        }

        else if (!isSeatAvailable(seat_number))
        {
            red();
            printf("Seat %d is already booked.\n", seat_number);
            reset();
            i--;
            continue;
        }

        struct Ticket new_ticket = {ticket_count + 1, "", seat_number};
        strncpy(new_ticket.passenger_name, name, MAX_NAME_LEN - 1);
        tickets[ticket_count++] = new_ticket;
        enqueueTicket(new_ticket);
        pushToStack(new_ticket);

        char action[MAX_NAME_LEN + 50];
        sprintf(action, "Booked ticket: ID: %d, Name: %s, Seat: %d", new_ticket.ticket_id, name, seat_number);
        addToHistory(action);

        green();
        printf("Ticket booked successfully.\n");
        reset();
    }

    ticket_price(num_tickets);
}

void displayTickets()
{
    if (ticket_count == 0)
    {
        red();
        printf("No tickets booked yet.\n");
        reset();
        return;
    }
    purple();
    printf("List of Tickets:\nID\tName\tSeat\n");
    reset();
    for (int i = 0; i < ticket_count; i++)
    {
        printf("%d\t%s\t%d\n", tickets[i].ticket_id, tickets[i].passenger_name, tickets[i].seat_number);
    }
}
void cancelTicket()
{
    int seat_number;
    printf("Enter seat number to cancel: ");
    scanf("%d", &seat_number);

    int found = 0;
    for (int i = 0; i < ticket_count; i++)
    {
        if (tickets[i].seat_number == seat_number)
        {
            green();
            printf("Cancelling ticket: ID: %d, Name: %s, Seat: %d\n", tickets[i].ticket_id, tickets[i].passenger_name, tickets[i].seat_number);
            reset();
            printf("\nReturned amount: %d\n", TICKET_PRICE);

            // Adding to history
            char action[MAX_NAME_LEN + 50];
            sprintf(action, "Cancelled ticket: ID: %d, Name: %s, Seat: %d", tickets[i].ticket_id, tickets[i].passenger_name, tickets[i].seat_number);
            addToHistory(action);

            // Shift all tickets one step to the left
            for (int j = i; j < ticket_count - 1; j++)
            {
                tickets[j] = tickets[j + 1];
            }

            green();
            printf("\nThanks for using our system!\n");
            reset();
            ticket_count--;
            found = 1;
            break;
        }
    }

    if (!found)
    {
        red();
        printf("No ticket found for seat number: %d\n", seat_number);
        reset();
    }
}

// Sorting Tickets
void sortTicketsBySeat()
{
    for (int i = 0; i < ticket_count - 1; i++)
    {
        for (int j = i + 1; j < ticket_count; j++)
        {
            if (tickets[i].seat_number > tickets[j].seat_number)
            {
                struct Ticket temp = tickets[i];
                tickets[i] = tickets[j];
                tickets[j] = temp;
            }
        }
    }
    green();
    printf("Tickets sorted by seat number.\n");
    reset();
}

// Searching for a Ticket by Name
void searchTicketByName()
{
    char name[MAX_NAME_LEN];
    printf("Enter passenger name to search: ");
    scanf("%s", name);

    int found = 0;
    for (int i = 0; i < ticket_count; i++)
    {
        if (strcmp(tickets[i].passenger_name, name) == 0)
        {
            green();
            printf("Ticket Found:\nID: %d, Name: %s, Seat: %d\n", tickets[i].ticket_id, tickets[i].passenger_name, tickets[i].seat_number);
            reset();
            found = 1;
        }
    }

    if (!found)
    {
        red();
        printf("No ticket found for passenger: %s\n", name);
        reset();
    }
}
void adminPanel()
{
    char password[50];
    printf("Enter admin password: ");
    scanf("%s", password);

    if (strcmp(password, ADMIN_PASSWORD) == 0)
    {
        green();
        printf("Access granted.\n");
        reset();
        int admin_choice;

        do
        {

            printf("\n~~~~ Admin Panel ~~~~\n\n");
            printf("1. Display All Tickets\n");
            printf("2. Sort Tickets by Seat Number\n");
            printf ("3. View Total History\n");
            printf("4. Exit Admin Panel\n");
            printf("\nEnter your choice: ");
            scanf("%d", &admin_choice);

            switch (admin_choice)
            {
            case 1:
                displayTickets();
                break;
            case 2:
                sortTicketsBySeat();
                displayTickets();
                break;
            case 3:
                displayHistory ();
                break;
            case 4:
            {
                green();
                printf("Exiting admin panel.\n");
                reset();
                break;
            }
            default:
            {
                red();
                printf("Invalid choice. Please try again.\n");
                reset();
            }
            }
        }
        while (admin_choice != 4);

    }
    else
    {
        red();
        printf("Incorrect password. Access denied.\n");
        reset();
    }
}



//Main Function
int main()
{
    int choice;
    do
    {
        cyan();
        printf("\n~~~ Ticket Booking System ~~~\n\n");
        reset();
        printf("1. Book a Ticket\n2. Display Tickets\n3. Cancel Ticket\n4. Search Ticket (Name)\n5. Admin Panel\n6. Exit\n");
        printf("\nEnter choice: ");
        scanf("%d", &choice);

        switch (choice)
        {
        case 1:
            addTicket();
            break;
        case 2:
            displayTickets();
            break;
        case 3:
            cancelTicket ();
            break;
        case 4:
            searchTicketByName();
            break;
        case 5:
            adminPanel();
            break;
        case 6:
            green();
            printf("\nGoodbye! Have a nice day.\n");
            reset();
            break;

        default:
            red();
            printf("Invalid choice.\n");
            reset();
        }
    }
    while (choice != 6);

    getch ();
    return 0;
}
