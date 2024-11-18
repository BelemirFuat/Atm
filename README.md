# An atm simulator written in c++

---

## Basic features
- server-client logic for data communication
- data encrypt for security
- database.cpp for handling database files externally on hard drive. ( I just wanted it to be simple so I didn't use SQL or any dbms)
- This project's goal is to be able to make transactions, real time account watch... anything that a real atm can do.
---

## 
- actually securityServer doesn't anything. 

## TODO :
[ ] Succesfully login.
[ ] Succesfully logout.
[ ] Succesfully access the database from atmClient side.
[ ] Succesfully Make transactions
[ ] Succesfully write data into database
...

## tecnic features
### FEATURES
1. online account system
2. interactive transaction system
3. encrypted log system for each user and bank itself
4. encrypted account info
5. secure authentication between server and client. 
6. separete secure database program

---

### SERVER AND CLIENT COM
1. userName and pin CLIENT -> SERVER
2. userName and pin true or false SERVER->CLIENT

---

###DATABASE AND SERVER COM
1. userName and pin DATABASE -> SERVER

---

## DATABASE DATA STORE LOGIC
1. DATABASE creates an unique hash code for username and puts it to a vector named usersTable with datas userName and a number of users self object in the users vector.
2. users self object holds datas : userName, PIN, internetPIN, vector<accounts>, vector<cards>
3. database commands:
                 - quit
                 - query username pin
                 - addUser userName pin
                 - shutdown
                 - deleteUser
                 - infoUser

4. returns: 
                  - 401 : login unsuccessful
                  - 400 : Bad request. Missing parameters.";
                  - 200 : login successful

---

### SERVER COMMANDS
1. login(userName, PIN)
2. showAccountInfo(userName, PIN, authToken)


### PORT NUMBERS
- 8000 : security server
- 8001 : database server
- 8002 : main server

