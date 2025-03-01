/*
 *  Project A: Multi-Threading Implementation
 *  Name: Brien Kim
 *  Course: CS 3502 Section W03
 *  NetID: bkim50
 *
 *  Overview: this file is a main task for Project A: Multi-Threading Implementation
 */

#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <functional>

#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

/*
    This program adapted the banking system (the example provided from the instruction)
    to properly demonstrate and test the multi-threading implementation.

    As the banking system, the program contains two classes:
    - Account class: it behaves as a banking account; it has an unique id and account balance for each object created
    - Bank class: it behaves as a banking system; it enables transaction between Account using threads
*/

class Account
{
private:
    // a simple unique indentifier for each Account
    static int tracking_account_number;
    int account_number;
    double balance;

public:
    // default constructor
    Account()
    {
        this->balance = 10000;
    }

    // overloaded constructor
    Account(double balance)
    {
        this->account_number = tracking_account_number++;
        this->balance = balance;
    }

    // add a specific amount of money into Account
    void addBalance(double amount)
    {
        balance += amount;
    }

    // remove a specific amount of money from Account
    void removeBalance(double amount)
    {
        balance -= amount;
    }

    // retrieve a unique id for Account
    int getAccountNumber()
    {
        return account_number;
    }

    // retrieve a balance for Account
    int getBalance()
    {
        return balance;
    }
};

/*
    Bank class has following transactions:
    - deposit: add a specific amount of money into Account
    - withdraw: remove a specific amount of money from Account
    - transfer: move a specific amount of money to other Account
*/
class Bank
{
private:
    // since there will be only 1 Bank, all attributes are non-static

    /*
        'timed_mutex' is still "mutual exclusion lock", but allows to specify a timeout

        Resource Ordering to prevent deadlock:
        'account_numbers_mutex_lock' > 'accounts_mutex_lock' > 'deposit_mutex_lock' > 'withdrawl_mutex_lock'
    */
    timed_mutex account_numbers_mutex_lock, accounts_mutex_lock, deposit_mutex_lock, withdrawl_mutex_lock;

    // these attributes are shared resources among all threads
    // threads do only access
    vector<int> account_numbers;
    map<int, Account> accounts;

    // these attributes are shared resources among all threads
    // thread do both access and modify
    double total_deposit = 0;
    double total_withdrawl = 0;

public:
    // default constructor
    Bank() {}

    // deposit a specific amount of money into a specific Account
    void deposit(int account_number, double amount)
    {

    critical_section:
    {
        // time frame for timeout: 1,000 ms (1 second)
        chrono::milliseconds timeout(1000);

        /*
            lock acquisition for 'account_numbers'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!account_numbers_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            account_numbers_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        /*
            lock acquisition for 'accounts'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!accounts_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            accounts_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        /*
            lock acquisition for 'total_deposit'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!deposit_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            deposit_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        // if 'account_number' does not exist, then terminate the function
        // a potential race condition, since 'account_numbers' is a shared resource
        // and it can be acccessed by numerous threads simultaneously if there is no mutex
        if (find(account_numbers.begin(), account_numbers.end(), account_number) == account_numbers.end())
        {
            cout << "Account " << account_number << " does not exist" << endl;
            return;
        }

        // if 'amount' is negative, then terminate the function
        if (amount < 0)
        {
            cout << "Invalid amount of money" << endl;
            return;
        }

        // a potential race condition, since 'accounts' is a shared resource
        // and it can be acccessed by numerous threads simultaneously if there is no mutex
        Account account = accounts[account_number];
        account.addBalance(amount);

        // a potential race condition, since 'total_deposit' is a shared resource
        // and it can be acccessed and modified by numerous threads simultaneously if there is no mutex
        total_deposit += amount;
        cout << "Thread " << this_thread::get_id() << " performs deposit" << endl;
        cout << "Account " << account_number << " deposits $" << amount << "\n"
             << endl;

        // lock release for 'account_numbers'
        account_numbers_mutex_lock.unlock();

        // lock release for 'accounts'
        accounts_mutex_lock.unlock();

        // lock release for 'total_deposit'
        deposit_mutex_lock.unlock();
    }
    }

    // withdraw a specific amount of money into a specific Account
    void withdraw(int account_number, double amount)
    {

    critical_section:
    {
        // time frame for timeout: 1,000 ms (1 second)
        chrono::milliseconds timeout(1000);

        /*
            lock acquisition for 'account_numbers'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!account_numbers_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            account_numbers_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        /*
            lock acquisition for 'accounts'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!accounts_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            accounts_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        /*
            lock acquisition for 'total_withdrawl'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!withdrawl_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            withdrawl_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        // if 'account_number' does not exist, then terminate the function
        // a potential race condition, since 'account_numbers' is a shared resource
        // and it can be acccessed by numerous threads simultaneously if there is no mutex
        if (find(account_numbers.begin(), account_numbers.end(), account_number) == account_numbers.end())
        {
            cout << "Account " << account_number << " does not exist" << endl;
            return;
        }

        // if 'amount' is negative, then terminate the function
        if (amount < 0)
        {
            cout << "Invalid amount of money" << endl;
            return;
        }

        // a potential race condition, since 'accounts' is a shared resource
        // and it can be acccessed by numerous threads simultaneously if there is no mutex
        Account account = accounts[account_number];

        // ensure that 'amount' does not exceed the account's 'balance'
        // otherwise, terminate the function
        if (account.getBalance() < amount)
        {
            cout << "Account " << account_number << " experiences overdraft";
            return;
        }

        account.removeBalance(amount);

        // a potential race condition, since 'total_withdrawl' is a shared resource
        // and it can be acccessed and modified by numerous threads simultaneously if there is no mutex
        total_withdrawl += amount;
        cout << "Thread " << this_thread::get_id() << " performs withdrawl" << endl;
        cout << "Account " << account_number << " withdraws $" << amount << "\n"
             << endl;

        // lock release for 'account_numbers'
        account_numbers_mutex_lock.unlock();

        // lock release for 'accounts'
        accounts_mutex_lock.unlock();

        // lock release for 'total_withdrawl'
        withdrawl_mutex_lock.unlock();
    }
    }

    // tranfer a specific amount of money from Account to another Account
    void transfer(int sender_account_number, int receiver_account_number, double amount)
    {

    critical_section:
    {
        // time frame for timeout: 1,000 ms (1 second)
        chrono::milliseconds timeout(1000);

        /*
            lock acquisition for 'account_numbers'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!account_numbers_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            account_numbers_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        /*
            lock acquisition for 'accounts'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!accounts_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            accounts_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        /*
            lock acquisition for 'total_deposit'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!deposit_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            deposit_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        /*
            lock acquisition for 'total_withdrawl'
            timeout mechanism is adapted

            IF the lock is not acquired within the time frame,
            THEN it detects a potential deadlock occurrence
            THUS, as of deadlock recovery,
            - it promptly release the locked mutex
            - then, re-attempt (or rollback) the performance after wait 1,000 ms
        */
        if (!withdrawl_mutex_lock.try_lock_for(timeout))
        {
            cout << "Potential Deadlock for Thread " << this_thread::get_id() << endl;
            cout << "Re-attempting the performance" << endl;
            withdrawl_mutex_lock.unlock();
            this_thread::sleep_for(chrono::milliseconds(1000));
            goto critical_section;
        }

        // if 'sender_account_number' or 'receiver_account_number' does not exist, then terminate the function
        // a potential race condition, since 'account_numbers' is a shared resource
        // and it can be acccessed by numerous threads simultaneously if there is no mutex
        if (find(account_numbers.begin(), account_numbers.end(), sender_account_number) == account_numbers.end())
        {
            cout << "Account " << sender_account_number << " does not exist" << endl;
            return;
        }
        else if (find(account_numbers.begin(), account_numbers.end(), receiver_account_number) == account_numbers.end())
        {
            cout << "Account " << receiver_account_number << " does not exist" << endl;
            return;
        }

        // if 'amount' is negative, then terminate the function
        if (amount < 0)
        {
            cout << "Invalid amount of money" << endl;
            return;
        }

        // a potential race condition, since 'accounts' is a shared resource
        // and it can be acccessed by numerous threads simultaneously if there is no mutex
        Account sender = accounts[sender_account_number];

        // ensure that 'amount' does not exceed a sender's 'balance'
        // otherwise, terminate the function
        if (sender.getBalance() < amount)
        {
            cout << "Account " << sender_account_number << " experiences overdraft";
            return;
        }

        // a potential race condition, since 'accounts' is a shared resource
        // and it can be acccessed by numerous threads simultaneously if there is no mutex
        Account receiver = accounts[receiver_account_number];

        receiver.addBalance(amount);
        sender.removeBalance(amount);

        // a potential race condition, since 'total_deposit' and 'total_withdrawl' are shared resources
        // and these can be acccessed and modified by numerous threads simultaneously if there is no mutex
        total_deposit += amount;
        total_withdrawl += amount;
        cout << "Thread " << this_thread::get_id() << " performs transfer" << endl;
        cout << "Account " << sender_account_number << " transfers $" << amount << " to Account " << receiver_account_number << "\n"
             << endl;

        // lock release for 'account_numbers'
        account_numbers_mutex_lock.unlock();

        // lock release for 'accounts'
        accounts_mutex_lock.unlock();

        // lock release for 'total_deposit'
        deposit_mutex_lock.unlock();

        // lock release for 'total_withdrawl'
        withdrawl_mutex_lock.unlock();
    }
    }

    // add an Account into the map
    void addAccount(Account account)
    {
        account_numbers.push_back(account.getAccountNumber());
        accounts[account.getAccountNumber()] = account;
    }

    // retrieve the vector of existing account numbers
    vector<int> getAllAccountNumbers()
    {
        return account_numbers;
    }

    // retrieve the total amount that has been deposited by all Account
    double getTotalDeposit()
    {
        return total_deposit;
    }

    // retrieve the total amount that has been withdrew by all Account
    double getTotalWithdrawl()
    {
        return total_withdrawl;
    }
};

int Account::tracking_account_number;

// the main function for program execution
int main()
{
    // collect a start time for the program
    auto time_start = chrono::high_resolution_clock::now();

    cout << "\nMain thread (parent) " << this_thread::get_id() << " is started\n"
         << endl;

    // assume there are 5 Accounts registered in Bank
    Bank bank;

    // prompt a number of accounts, 'n'
    int num_of_accounts;
    cout << "Enter a number of accounts registered in the bank: ";

    // ensure the input is a positive number (integer)
    while (!(cin >> num_of_accounts) || num_of_accounts <= 0)
    {
        cout << "Invalid Input: enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // assume there are 'n' accounts registered in the bank
    for (int i = 0; i < num_of_accounts; i++)
    {
        // add Account with an arbitrary balance between 10,000 and 50,000 (inclusive)
        bank.addAccount(Account((rand() % 40001) + 10000));
    }

    // prompt a number of threads, 'n'
    // threads may represent each customer's interaction (or each customer)
    int num_of_threads;
    cout << "Enter a number of threads: ";

    // ensure the input is a positive number (integer)
    while (!(cin >> num_of_threads) || num_of_threads <= 0)
    {
        cout << "Invalid Input: enter a number: ";
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    // break a new line
    cout << "\n"
         << endl;

    // an array of threads
    thread threads[num_of_threads];

    /*
        assume there are 'n' threads for each transaction made by customers with their corresponding Accounts

        Since pragmatic banking transactions can not be designed without actual user interactions,
        for desmonstration and testing, account numbers will be selected sequentially
        and their transactions and an amount of money will be selected arbitrarily for each thread.
    */
    vector<int> account_numbers = bank.getAllAccountNumbers();

    for (int id = 0; id < num_of_threads; id++)
    {
        // randomly select a transaction by randomly selecting between 0 and 2 (inclusive)
        // 0 = deposit
        // 1 = withdraw
        // 2 - transfer
        int transaction = (rand() % 3);

        // sequentially select a account number
        // if there are more threads, then account number will be selected again from beginning
        int account_number = account_numbers[id % account_numbers.size()];

        // generate a amount of money randomly between 1,000 and 10,000 (inclusive)
        double amount = (rand() % 9001) + 1000;

        // randomly select a transaction for each thread
        if (transaction == 0)
        {
            threads[id] = thread(bind(&Bank::deposit, &bank, account_number, amount));
        }
        else if (transaction == 1)
        {
            threads[id] = thread(bind(&Bank::withdraw, &bank, account_number, amount));
        }
        else if (transaction == 2)
        {
            // select a next account number as a receiver
            int receiver = account_numbers[(id + 1) % account_numbers.size()];
            threads[id] = thread(bind(&Bank::transfer, &bank, account_number, receiver, amount));
        }
        else
        {
            cout << "Invalid transaction" << endl;
        }
    }

    // wait for all of processing threads to terminate
    for (int id = 0; id < num_of_threads; id++)
    {
        threads[id].join();
    }

    // display results
    cout << "\nTotal Deposit: $" << bank.getTotalDeposit() << endl;
    cout << "Total Withdrawl: $" << bank.getTotalWithdrawl() << endl;

    // collect a end time for the program
    auto time_end = chrono::high_resolution_clock::now();

    // calculate estimated excution time for the program
    auto execution_time = chrono::duration_cast<chrono::milliseconds>(time_end - time_start);

    cout << "\nMain thread (parent) " << this_thread::get_id() << " is finished\n"
         << endl;

    // display the estimated execution time
    cout << "Estimated Execution Time: " << execution_time.count() << " milliseconds (ms)\n"
         << endl;

    return 0;
}