# Implement a association rule mining algorithm

Given a input DB, mine strong association rules from the input DB with user-defined min_supp and min_conf

## Input
## User input
* min_supp: minimum support in percentage. 
Support(B) = (Transactions containing (B))/(Total Transactions)
* min_conf: minimum confidence in percentage. 
Confidence(A→B) = (Transactions containing both (A and B))/(Transactions containing A)

> Example User input: 
> Enter min_supp(%): 20 
> Enter min_conf(%): 50

### The input DB format is following
* Each line is a transaction
* Each number is a item 
* There are many items in a transaction
> Example transaction: 39 73 74 75 76 77 78 79 

## Output the strong association rules 
> Example strong association rule: Rule {48} -> {39} Confidence (29142 / 42135)

