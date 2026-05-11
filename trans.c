#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// structure
struct clientData
{
    unsigned int acctNum;
    char lastName[15];
    char firstName[10];
    double balance;
};

// prototypes
unsigned int enterChoice(void);
void textFile(FILE *readPtr);
void updateRecord(FILE *fPtr);
void newRecord(FILE *fPtr);
void deleteRecord(FILE *fPtr);
void searchByName(FILE *fPtr);
void transferMoney(FILE *fPtr);
void totalBalance(FILE *fPtr);
void printSingle(FILE *fPtr);

int main()
{
    FILE *cfPtr;
    unsigned int choice;
    char password[10];

    printf("Enter admin password: ");
    scanf("%9s", password);

    if (strcmp(password, "admin123") != 0)
    {
        printf("Access denied!\n");
        return 1;
    }

    // open binary file
    cfPtr = fopen("credit.dat", "rb+");

    // create file if not exists
    if (cfPtr == NULL)
    {
        cfPtr = fopen("credit.dat", "wb+");

        if (cfPtr == NULL)
        {
            printf("File could not be opened.\n");
            return 1;
        }

        // initialize 100 blank records
        struct clientData blank = {0, "", "", 0.0};

        for (int i = 0; i < 100; i++)
        {
            fwrite(&blank, sizeof(struct clientData), 1, cfPtr);
        }
    }

    while ((choice = enterChoice()) != 9)
    {
        switch (choice)
        {
        case 1:
            textFile(cfPtr);
            break;

        case 2:
            updateRecord(cfPtr);
            break;

        case 3:
            newRecord(cfPtr);
            break;

        case 4:
            deleteRecord(cfPtr);
            break;

        case 5:
            searchByName(cfPtr);
            break;

        case 6:
            transferMoney(cfPtr);
            break;

        case 7:
            totalBalance(cfPtr);
            break;

        case 8:
            printSingle(cfPtr);
            break;

        default:
            printf("Invalid choice\n");
        }
    }

    fclose(cfPtr);

    return 0;
}

// MENU
unsigned int enterChoice(void)
{
    unsigned int choice;

    printf("\n========== BANK MANAGEMENT SYSTEM ==========\n");
    printf("1 - Export to text file\n");
    printf("2 - Update account\n");
    printf("3 - Add new account\n");
    printf("4 - Delete account\n");
    printf("5 - Search by last name\n");
    printf("6 - Transfer money\n");
    printf("7 - Display total balance\n");
    printf("8 - Print single account\n");
    printf("9 - Exit\n");
    printf("Enter your choice: ");

    scanf("%u", &choice);

    return choice;
}

// EXPORT FILE
void textFile(FILE *readPtr)
{
    FILE *writePtr;
    struct clientData client;

    writePtr = fopen("accounts.txt", "w");

    if (writePtr == NULL)
    {
        printf("Cannot create text file.\n");
        return;
    }

    rewind(readPtr);

    fprintf(writePtr,
            "%-6s%-16s%-11s%10s\n",
            "Acct",
            "Last Name",
            "First Name",
            "Balance");

    while (fread(&client, sizeof(struct clientData), 1, readPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            fprintf(writePtr,
                    "%-6u%-16s%-11s%10.2f\n",
                    client.acctNum,
                    client.lastName,
                    client.firstName,
                    client.balance);
        }
    }

    fclose(writePtr);

    printf("Data exported to accounts.txt successfully.\n");
}

// UPDATE ACCOUNT
void updateRecord(FILE *fPtr)
{
    unsigned int acc;
    double amount;
    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &acc);

    if (acc < 1 || acc > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (acc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    printf("Current balance: %.2f\n",
           client.balance);

    printf("Enter amount (+ deposit / - withdraw): ");
    scanf("%lf", &amount);

    if (client.balance + amount < 0)
    {
        printf("Insufficient funds.\n");
        return;
    }

    client.balance += amount;

    // corrected warning line
    fseek(fPtr,
          -((long)sizeof(struct clientData)),
          SEEK_CUR);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Account updated successfully.\n");
}

// ADD NEW ACCOUNT
void newRecord(FILE *fPtr)
{
    struct clientData client = {0, "", "", 0.0};
    unsigned int acc;

    printf("Enter new account number (1-100): ");
    scanf("%u", &acc);

    if (acc < 1 || acc > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (acc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum != 0)
    {
        printf("Account already exists.\n");
        return;
    }

    printf("Enter lastname firstname balance:\n");

    scanf("%14s %9s %lf",
          client.lastName,
          client.firstName,
          &client.balance);

    client.acctNum = acc;

    fseek(fPtr,
          (acc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&client,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("New account created successfully.\n");
}

// DELETE ACCOUNT
void deleteRecord(FILE *fPtr)
{
    struct clientData client;
    struct clientData blank = {0, "", "", 0.0};

    unsigned int acc;

    printf("Enter account number to delete: ");
    scanf("%u", &acc);

    if (acc < 1 || acc > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (acc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
        return;
    }

    fseek(fPtr,
          (acc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&blank,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Account deleted successfully.\n");
}

// SEARCH BY LAST NAME
void searchByName(FILE *fPtr)
{
    struct clientData client;
    char name[15];
    int found = 0;

    printf("Enter last name to search: ");
    scanf("%14s", name);

    rewind(fPtr);

    printf("\n%-6s%-16s%-11s%10s\n",
           "Acct",
           "Last Name",
           "First Name",
           "Balance");

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.acctNum != 0 &&
            strcmp(client.lastName, name) == 0)
        {
            printf("%-6u%-16s%-11s%10.2f\n",
                   client.acctNum,
                   client.lastName,
                   client.firstName,
                   client.balance);

            found = 1;
        }
    }

    if (!found)
    {
        printf("No matching account found.\n");
    }
}

// TRANSFER MONEY
void transferMoney(FILE *fPtr)
{
    unsigned int fromAcc, toAcc;
    double amount;

    struct clientData fromClient;
    struct clientData toClient;

    printf("Enter source account number: ");
    scanf("%u", &fromAcc);

    printf("Enter destination account number: ");
    scanf("%u", &toAcc);

    printf("Enter amount to transfer: ");
    scanf("%lf", &amount);

    if (amount <= 0)
    {
        printf("Invalid transfer amount.\n");
        return;
    }

    fseek(fPtr,
          (fromAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&fromClient,
          sizeof(struct clientData),
          1,
          fPtr);

    fseek(fPtr,
          (toAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&toClient,
          sizeof(struct clientData),
          1,
          fPtr);

    if (fromClient.acctNum == 0 ||
        toClient.acctNum == 0)
    {
        printf("Invalid account number.\n");
        return;
    }

    if (fromClient.balance < amount)
    {
        printf("Insufficient balance.\n");
        return;
    }

    fromClient.balance -= amount;
    toClient.balance += amount;

    fseek(fPtr,
          (fromAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&fromClient,
           sizeof(struct clientData),
           1,
           fPtr);

    fseek(fPtr,
          (toAcc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fwrite(&toClient,
           sizeof(struct clientData),
           1,
           fPtr);

    printf("Money transferred successfully.\n");
}

// TOTAL BALANCE
void totalBalance(FILE *fPtr)
{
    struct clientData client;
    double total = 0.0;

    rewind(fPtr);

    while (fread(&client,
                 sizeof(struct clientData),
                 1,
                 fPtr) == 1)
    {
        if (client.acctNum != 0)
        {
            total += client.balance;
        }
    }

    printf("Total bank balance = %.2f\n",
           total);
}

// PRINT SINGLE ACCOUNT
void printSingle(FILE *fPtr)
{
    unsigned int acc;
    struct clientData client;

    printf("Enter account number: ");
    scanf("%u", &acc);

    if (acc < 1 || acc > 100)
    {
        printf("Invalid account number.\n");
        return;
    }

    fseek(fPtr,
          (acc - 1) * sizeof(struct clientData),
          SEEK_SET);

    fread(&client,
          sizeof(struct clientData),
          1,
          fPtr);

    if (client.acctNum == 0)
    {
        printf("Account not found.\n");
    }
    else
    {
        printf("\n========== ACCOUNT DETAILS ==========\n");

        printf("Account Number : %u\n",
               client.acctNum);

        printf("Last Name      : %s\n",
               client.lastName);

        printf("First Name     : %s\n",
               client.firstName);

        printf("Balance        : %.2f\n",
               client.balance);
    }
}