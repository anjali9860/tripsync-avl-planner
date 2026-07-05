#include<stdio.h>
#include<string.h>
#include<stdlib.h>

/* ============================================================
   STRUCT DEFINITIONS
   ============================================================ */

typedef struct NavNode{
    int stepId;
    char directions[50];
    int distance;
    int cost;
    struct NavNode *left;
    struct NavNode *right;
    int height;
}NavNode;

typedef struct TripNode{
    int actId;
    char activityType[100];
    char location[100];
    int date;                   /* YYYYMMDD */
    int cost;
    int skipped;                /* 1 = not visited, 0 = visited */
    NavNode *navRoot;
    struct TripNode *left;
    struct TripNode *right;
    int height;
}TripNode;

typedef struct TripList{
    int tripId;
    char tripName[100];
    char source[50];
    char destination[50];
    int totalCost;
    TripNode *actRoot;
    struct TripList *next;
}TripList;




int max(int a, int b){
    return (a > b) ? a : b;
}

void clearScreen(){
    printf("\n");
}


/* ============================================================
   NAV NODE  
   ============================================================ */

int FindNavNode(NavNode *root, int id){
    if(root == NULL) return 0;
    if(id == root->stepId) return 1;
    if(id < root->stepId) return FindNavNode(root->left, id);
    return FindNavNode(root->right, id);
}

int heightNav(NavNode *node){
    return (node == NULL) ? 0 : node->height;
}

int getBalanceNav(NavNode *node){
    return (node == NULL) ? 0 : heightNav(node->left) - heightNav(node->right);
}

NavNode* rightRotateNav(NavNode* y){
    NavNode* x  = y->left;
    NavNode* T2 = x->right;
    x->right = y;
    y->left  = T2;
    y->height = max(heightNav(y->left), heightNav(y->right)) + 1;
    x->height = max(heightNav(x->left), heightNav(x->right)) + 1;
    return x;
}

NavNode* leftRotateNav(NavNode* x){
    NavNode* y  = x->right;
    NavNode* T2 = y->left;
    y->left  = x;
    x->right = T2;
    x->height = max(heightNav(x->left), heightNav(x->right)) + 1;
    y->height = max(heightNav(y->left), heightNav(y->right)) + 1;
    return y;
}

NavNode* insertNavHelper(NavNode* root, NavNode* newNode){
    if(root == NULL) return newNode;

    if(newNode->stepId < root->stepId)
        root->left  = insertNavHelper(root->left,  newNode);
    else if(newNode->stepId > root->stepId)
        root->right = insertNavHelper(root->right, newNode);
    else
        return root;   // duplicate //

    root->height = 1 + max(heightNav(root->left), heightNav(root->right));
    int balance  = getBalanceNav(root);

    if(balance > 1  && newNode->stepId < root->left->stepId)  return rightRotateNav(root);
    if(balance < -1 && newNode->stepId > root->right->stepId) return leftRotateNav(root);
    if(balance > 1  && newNode->stepId > root->left->stepId){
        root->left = leftRotateNav(root->left);  return rightRotateNav(root);
    }
    if(balance < -1 && newNode->stepId < root->right->stepId){
        root->right = rightRotateNav(root->right); return leftRotateNav(root);
    }
    return root;
}

NavNode* CreateNavNode(NavNode *root){
    int id;
    do{
        printf("  Enter Step-ID  : ");
        scanf("%d", &id);

        if(FindNavNode(root, id))
            printf("  ID already exists. Re-enter.\n");
    } while(FindNavNode(root, id));

    NavNode *n = (NavNode*)malloc(sizeof(NavNode));
    n->stepId  = id;
    printf("  Enter Direction: ");
    scanf(" %[^\n]", n->directions);
    printf("  Enter Distance : ");
    scanf("%d", &n->distance);
    printf("  Enter Cost     : ");
    scanf("%d", &n->cost);
    n->left = n->right = NULL;
    n->height = 1;
    return n;
}

NavNode* insertNavNode(NavNode* root){
    NavNode* newNode = CreateNavNode(root);
    return insertNavHelper(root, newNode);
}

void displayNavInorder(NavNode *root){
    if(root == NULL) return;
    displayNavInorder(root->left);
    printf("    Step ID   : %d\n", root->stepId);
    printf("    Direction : %s\n", root->directions);
    printf("    Distance  : %d km\n", root->distance);
    printf("    Cost      : %d\n", root->cost);
    printf("    -------------------------\n");
    displayNavInorder(root->right);
}

NavNode* minValueNavNode(NavNode* root){
    while(root->left != NULL) root = root->left;
    return root;
}

NavNode* deleteNavNode(NavNode* root, int id){
    if(root == NULL) return root;

    if(id < root->stepId)       root->left  = deleteNavNode(root->left, id);
    else if(id > root->stepId)  root->right = deleteNavNode(root->right, id);
    else{
        if(root->left == NULL && root->right == NULL){ free(root); return NULL; }
        if(root->left == NULL || root->right == NULL){
            NavNode* t = root->left ? root->left : root->right;
            free(root); return t;
        }
        NavNode* t = minValueNavNode(root->right);
        root->stepId = t->stepId;
        strcpy(root->directions, t->directions);
        root->distance = t->distance;
        root->cost     = t->cost;
        root->right    = deleteNavNode(root->right, t->stepId);
    }

    root->height = 1 + max(heightNav(root->left), heightNav(root->right));
    int balance  = getBalanceNav(root);

    if(balance > 1  && getBalanceNav(root->left) >= 0)  return rightRotateNav(root);
    if(balance > 1  && getBalanceNav(root->left) < 0){
        root->left = leftRotateNav(root->left);  return rightRotateNav(root);
    }
    if(balance < -1 && getBalanceNav(root->right) <= 0) return leftRotateNav(root);
    if(balance < -1 && getBalanceNav(root->right) > 0){
        root->right = rightRotateNav(root->right); return leftRotateNav(root);
    }
    return root;
}

NavNode* updateNavNode(NavNode* root, int id){
    if(root == NULL){ printf("  Step not found!\n"); return NULL; }

    if(id < root->stepId)       root->left  = updateNavNode(root->left, id);
    else if(id > root->stepId)  root->right = updateNavNode(root->right, id);
    else{
        printf("  Enter new Direction: ");
        scanf(" %[^\n]", root->directions);
        printf("  Enter new Distance : ");
        scanf("%d", &root->distance);
        printf("  Enter new Cost     : ");
        scanf("%d", &root->cost);
        printf("  Nav step updated successfully!\n");
    }
    return root;
}

void freeNavTree(NavNode* root){
    if(root == NULL) return;
    freeNavTree(root->left);
    freeNavTree(root->right);
    free(root);
}

void storeInorder(NavNode* root, NavNode* arr[], int *index){
    if(root == NULL) return;
    storeInorder(root->left, arr, index);
    arr[*index] = root;
    (*index)++;
    storeInorder(root->right, arr, index);
}

void searchDirectionBetween(NavNode* root){
    if(root == NULL){ printf("  No navigation data!\n"); return; }

    int source, dest;
    char targetDir[50];

    printf("  Enter Source Step ID     : ");  scanf("%d", &source);
    printf("  Enter Destination Step ID: ");  scanf("%d", &dest);
    printf("  Enter Direction to search: ");  scanf(" %[^\n]", targetDir);

    NavNode* arr[100];
    int n = 0;
    storeInorder(root, arr, &n);

    int srcIdx = -1, dstIdx = -1, i;
    for(i = 0; i < n; i++){
        if(arr[i]->stepId == source)  srcIdx = i;
        if(arr[i]->stepId == dest)    dstIdx = i;
    }

    if(srcIdx == -1 || dstIdx == -1){ printf("  Invalid source or destination!\n"); return; }
    if(srcIdx > dstIdx)              { printf("  Invalid path order!\n"); return; }

    for(i = srcIdx; i <= dstIdx; i++){
        if(strcmp(arr[i]->directions, targetDir) == 0){
            printf("  Direction FOUND at Step ID: %d\n", arr[i]->stepId);
            return;
        }
    }
    printf("  Direction NOT found in given path.\n");
}


/* ============================================================
   TRIP NODE  
   ============================================================ */

int FindTripNode(TripNode *root, int id){
    if(root == NULL) return 0;
    if(id == root->actId) return 1;
    if(id < root->actId) return FindTripNode(root->left, id);
    return FindTripNode(root->right, id);
}

int heightTrip(TripNode *node){
    return (node == NULL) ? 0 : node->height;
}

int getBalanceTrip(TripNode *node){
    return (node == NULL) ? 0 : heightTrip(node->left) - heightTrip(node->right);
}

TripNode* rightRotateTrip(TripNode* y){
    TripNode* x  = y->left;
    TripNode* T2 = x->right;
    x->right = y;
    y->left  = T2;
    y->height = max(heightTrip(y->left), heightTrip(y->right)) + 1;
    x->height = max(heightTrip(x->left), heightTrip(x->right)) + 1;
    return x;
}

TripNode* leftRotateTrip(TripNode* x){
    TripNode* y  = x->right;
    TripNode* T2 = y->left;
    y->left  = x;
    x->right = T2;
    x->height = max(heightTrip(x->left), heightTrip(x->right)) + 1;
    y->height = max(heightTrip(y->left), heightTrip(y->right)) + 1;
    return y;
}

TripNode* insertTripHelper(TripNode* root, TripNode* newNode){
    if(root == NULL) return newNode;

    if(newNode->date < root->date)       root->left  = insertTripHelper(root->left,  newNode);
    else if(newNode->date > root->date)  root->right = insertTripHelper(root->right, newNode);
    else return root;   /* duplicate date */

    root->height = 1 + max(heightTrip(root->left), heightTrip(root->right));
    int balance  = getBalanceTrip(root);

    if(balance > 1  && newNode->date < root->left->date)  return rightRotateTrip(root);
    if(balance < -1 && newNode->date > root->right->date) return leftRotateTrip(root);
    if(balance > 1  && newNode->date > root->left->date){
        root->left = leftRotateTrip(root->left);  return rightRotateTrip(root);
    }
    if(balance < -1 && newNode->date < root->right->date){
        root->right = rightRotateTrip(root->right); return leftRotateTrip(root);
    }
    return root;
}

TripNode* getTripNode(TripNode *root, int id){
    if(root == NULL) return NULL;
    if(id == root->actId) return root;
    if(id < root->actId)  return getTripNode(root->left, id);
    return getTripNode(root->right, id);
}

TripNode* minValueTripNode(TripNode* root){
    while(root->left != NULL) root = root->left;
    return root;
}

void freeNavTree(NavNode* root);  /* forward decl */

void freeTripTree(TripNode* root){
    if(root == NULL) return;
    freeTripTree(root->left);
    freeTripTree(root->right);
    freeNavTree(root->navRoot);
    free(root);
}

TripNode* CreateTripNode(TripNode *root){
    int id;
    do{
        printf("  Enter Activity ID  : ");
        scanf("%d", &id);
        if(FindTripNode(root, id))
            printf("  ID exists! Re-enter.\n");
    } while(FindTripNode(root, id));

    TripNode *n = (TripNode*)malloc(sizeof(TripNode));
    n->actId = id;
    printf("  Enter Activity Type (hotel/flight/visit): ");
    scanf(" %[^\n]", n->activityType);
    printf("  Enter Location     : ");
    scanf(" %[^\n]", n->location);
    printf("  Enter Date (YYYYMMDD): ");
    scanf("%d", &n->date);
    printf("  Enter Cost         : ");
    scanf("%d", &n->cost);
    printf("  Skipped? (1=Yes/0=No): ");
    scanf("%d", &n->skipped);

    n->navRoot = NULL;
    n->left = n->right = NULL;
    n->height = 1;

    int choice;
    printf("  Add navigation steps now? (1=Yes / 0=No): ");
    scanf("%d", &choice);
    while(choice == 1){
        n->navRoot = insertNavNode(n->navRoot);
        printf("  Add more steps? (1=Yes / 0=No): ");
        scanf("%d", &choice);
    }
    return n;
}

TripNode* insertTripNode(TripNode* root){
    TripNode* newNode = CreateTripNode(root);
    return insertTripHelper(root, newNode);
}

void displayTripInorder(TripNode *root){
    if(root == NULL) return;
    displayTripInorder(root->left);

    printf("\n  +--------------------------------------+\n");
    printf("  | Activity ID : %-5d                  |\n", root->actId);
    printf("  +--------------------------------------+\n");
    printf("  | Type     : %-29s|\n", root->activityType);
    printf("  | Location : %-29s|\n", root->location);
    printf("  | Date     : %-29d|\n", root->date);
    printf("  | Cost     : %-29d|\n", root->cost);
    printf("  | Skipped  : %-29s|\n", root->skipped ? "Yes" : "No");
    printf("  +--------------------------------------+\n");

    if(root->navRoot != NULL){
        printf("    -- Navigation Steps --\n");
        displayNavInorder(root->navRoot);
    } else {
        printf("    (No navigation steps)\n");
    }

    displayTripInorder(root->right);
}

TripNode* deleteTripNode(TripNode* root, int id){
    if(root == NULL) return root;

    if(id < root->actId)       root->left  = deleteTripNode(root->left, id);
    else if(id > root->actId)  root->right = deleteTripNode(root->right, id);
    else{
        if(root->left == NULL && root->right == NULL){
            freeNavTree(root->navRoot); free(root); return NULL;
        }
        if(root->left == NULL || root->right == NULL){
            TripNode* t = root->left ? root->left : root->right;
            freeNavTree(root->navRoot); free(root); return t;
        }
        TripNode* t = minValueTripNode(root->right);
        root->actId = t->actId;
        strcpy(root->activityType, t->activityType);
        strcpy(root->location, t->location);
        root->date    = t->date;
        root->cost    = t->cost;
        root->skipped = t->skipped;
        freeNavTree(root->navRoot);
        root->navRoot = t->navRoot;
        t->navRoot    = NULL;
        root->right   = deleteTripNode(root->right, t->actId);
    }

    root->height = 1 + max(heightTrip(root->left), heightTrip(root->right));
    int balance  = getBalanceTrip(root);

    if(balance > 1  && getBalanceTrip(root->left) >= 0)  return rightRotateTrip(root);
    if(balance > 1  && getBalanceTrip(root->left) < 0){
        root->left = leftRotateTrip(root->left);  return rightRotateTrip(root);
    }
    if(balance < -1 && getBalanceTrip(root->right) <= 0) return leftRotateTrip(root);
    if(balance < -1 && getBalanceTrip(root->right) > 0){
        root->right = rightRotateTrip(root->right); return leftRotateTrip(root);
    }
    return root;
}

void addNavigationToActivity(TripNode *root){
    if(root == NULL){ printf("  No activities present!\n"); return; }
    int id;
    do{
        printf("  Enter Activity ID: ");
        scanf("%d", &id);
        if(!FindTripNode(root, id)) printf("  Not found. Try again.\n");
    } while(!FindTripNode(root, id));

    TripNode *temp = getTripNode(root, id);
    int choice = 1;
    while(choice == 1){
        temp->navRoot = insertNavNode(temp->navRoot);
        printf("  Add more steps? (1=Yes / 0=No): ");
        scanf("%d", &choice);
    }
    printf("  Navigation added successfully!\n");
}

void countLocations(TripNode *root, char loc[][100], int freq[], int *size){
    if(root == NULL) return;
    countLocations(root->left, loc, freq, size);
    if(root->skipped == 0){
        int found = -1;
        for(int i = 0; i < *size; i++)
            if(strcmp(loc[i], root->location) == 0){ found = i; break; }
        if(found != -1) freq[found]++;
        else{ strcpy(loc[*size], root->location); freq[*size] = 1; (*size)++; }
    }
    countLocations(root->right, loc, freq, size);
}

void detectDuplicateLocations(TripNode *root){
    if(root == NULL){ printf("  No trip data!\n"); return; }
    char loc[100][100]; int freq[100], size = 0;
    countLocations(root, loc, freq, &size);
    int found = 0;
    printf("\n  --- Repeated Locations ---\n");
    for(int i = 0; i < size; i++)
        if(freq[i] > 1){ printf("  %s  visited %d times\n", loc[i], freq[i]); found = 1; }
    if(!found) printf("  No repeated locations.\n");
}

void collectHotels(TripNode *root, TripNode* arr[], int *size){
    if(root == NULL) return;
    collectHotels(root->left, arr, size);
    if(strcmp(root->activityType, "hotel") == 0 && root->skipped == 0)
        arr[(*size)++] = root;
    collectHotels(root->right, arr, size);
}

void sortHotelCosts(TripNode *root){
    if(root == NULL){ printf("  No trip data!\n"); return; }
    TripNode* arr[100]; int size = 0;
    collectHotels(root, arr, &size);
    if(size == 0){ printf("  No hotel data found!\n"); return; }
    for(int i = 0; i < size-1; i++)
        for(int j = 0; j < size-i-1; j++)
            if(arr[j]->cost < arr[j+1]->cost){
                TripNode* t = arr[j]; arr[j] = arr[j+1]; arr[j+1] = t;
            }
    printf("\n  --- Hotels Sorted by Cost (High to Low) ---\n");
    for(int i = 0; i < size; i++)
        printf("  %-25s  Cost: %d\n", arr[i]->location, arr[i]->cost);
}

int calculateNavCost(NavNode *root){
    if(root == NULL) return 0;
    return root->cost + calculateNavCost(root->left) + calculateNavCost(root->right);
}

int calculateTotalCost(TripNode *root){
    if(root == NULL) return 0;
    int total = calculateTotalCost(root->left);
    if(root->skipped == 0)
        total += root->cost + calculateNavCost(root->navRoot);
    total += calculateTotalCost(root->right);
    return total;
}

void updateTripActivity(TripNode *root){
    if(root == NULL){ printf("  No activities!\n"); return; }
    int id;
    printf("  Enter Activity ID to update: ");
    scanf("%d", &id);
    TripNode *node = getTripNode(root, id);
    if(node == NULL){ printf("  Activity not found!\n"); return; }
    printf("  Enter new Activity Type: ");
    scanf(" %[^\n]", node->activityType);
    printf("  Enter new Location     : ");
    scanf(" %[^\n]", node->location);
    printf("  Enter new Cost         : ");
    scanf("%d", &node->cost);
    printf("  Skipped? (1=Yes/0=No)  : ");
    scanf("%d", &node->skipped);
    printf("  Activity updated!\n");
}


/* ============================================================
   TRIP LIST  -  SINGLY LINKED LIST
   ============================================================ */

int findTripId(TripList *head, int id){
    while(head){ if(head->tripId == id) return 1; head = head->next; }
    return 0;
}

TripList* getTrip(TripList *head, int id){
    while(head){ if(head->tripId == id) return head; head = head->next; }
    return NULL;
}

TripList* addTrip(TripList *head){
    TripList *n = (TripList*)malloc(sizeof(TripList));
    int id;
    do{
        printf("  Enter Trip ID  : ");
        scanf("%d", &id);
        if(findTripId(head, id)) printf("  ID exists! Re-enter.\n");
    } while(findTripId(head, id));

    n->tripId = id;
    printf("  Enter Trip Name   : ");
    scanf(" %[^\n]", n->tripName);
    printf("  Enter Source      : ");
    scanf(" %[^\n]", n->source);
    printf("  Enter Destination : ");
    scanf(" %[^\n]", n->destination);
    n->totalCost = 0;
    n->actRoot   = NULL;
    n->next      = NULL;

    if(head == NULL) return n;
    TripList *curr = head;
    while(curr->next) curr = curr->next;
    curr->next = n;
    return head;
}

TripList* deleteTrip(TripList *head, int id){
    if(head == NULL){ printf("  No trips!\n"); return NULL; }
    if(head->tripId == id){
        TripList *t = head->next;
        freeTripTree(head->actRoot); free(head);
        printf("  Trip deleted!\n"); return t;
    }
    TripList *curr = head;
    while(curr->next){
        if(curr->next->tripId == id){
            TripList *t = curr->next;
            curr->next = t->next;
            freeTripTree(t->actRoot); free(t);
            printf("  Trip deleted!\n"); return head;
        }
        curr = curr->next;
    }
    printf("  Trip ID not found!\n");
    return head;
}

void displayAllTrips(TripList *head){
    if(head == NULL){ printf("  No trips available!\n"); return; }
    printf("\n  %-5s %-22s %-15s %-15s %s\n",
           "ID", "Trip Name", "From", "To", "Total Cost");
    printf("  ---------------------------------------------------------------\n");
    while(head){
        printf("  %-5d %-22s %-15s %-15s %d\n",
               head->tripId, head->tripName,
               head->source, head->destination, head->totalCost);
        head = head->next;
    }
}

/* stores TripNodes in date order into an array */
void storeTripInorder(TripNode *root, TripNode *arr[], int *size){
    if(root == NULL) return;
    storeTripInorder(root->left, arr, size);
    arr[(*size)++] = root;
    storeTripInorder(root->right, arr, size);
}

void findPath(TripNode *root){
    if(root == NULL){ printf("  No activities in this trip!\n"); return; }

    char source[100], destination[100];

    printf("  Enter Source Location     : ");
    scanf(" %[^\n]", source);
    printf("  Enter Destination Location: ");
    scanf(" %[^\n]", destination);

    /* collect all nodes in date order */
    TripNode *arr[100];
    int n = 0;
    storeTripInorder(root, arr, &n);

    int srcIdx = -1, dstIdx = -1;

    for(int i = 0; i < n; i++){
        if(strcasecmp(arr[i]->location, source) == 0 && srcIdx == -1)
            srcIdx = i;   /* first occurrence of source */
        if(strcasecmp(arr[i]->location, destination) == 0)
            dstIdx = i;   /* last occurrence of destination */
    }

    /* error checks */
    if(srcIdx == -1){
        printf("\n  Source '%s' not found in itinerary!\n", source);
        return;
    }
    if(dstIdx == -1){
        printf("\n  Destination '%s' not found in itinerary!\n", destination);
        return;
    }
    if(srcIdx == dstIdx){
        printf("\n  Source and destination are the same activity!\n");
        return;
    }
    if(srcIdx > dstIdx){
        printf("\n  No forward path found.\n");
        printf("  Hint: '%s' appears AFTER '%s' in your itinerary.\n",
               source, destination);
        return;
    }

    // path found - print it //
    printf("\n  =============================================\n");
    printf("  PATH  :  %s  -->  %s\n", source, destination);
    printf("  Steps :  %d activities\n", dstIdx - srcIdx + 1);
    printf("  =============================================\n");

    int totalCost = 0;

    for(int i = srcIdx; i <= dstIdx; i++){
        printf("\n  [Step %d]  %-10s  @ %-20s  Date: %d\n",
               i - srcIdx + 1,
               arr[i]->activityType,
               arr[i]->location,
               arr[i]->date);
        printf("           Cost: %-6d  | Skipped: %s\n",
               arr[i]->cost,
               arr[i]->skipped ? "Yes (not visited)" : "No");

        if(arr[i]->skipped == 0)
            totalCost += arr[i]->cost;

        if(arr[i]->navRoot != NULL){
            printf("           -- Directions --\n");
            displayNavInorder(arr[i]->navRoot);
            if(arr[i]->skipped == 0)
                totalCost += calculateNavCost(arr[i]->navRoot);
        } else {
            printf("           (No navigation steps)\n");
        }
    }

    printf("\n  =============================================\n");
    printf("  Total Path Cost (visited only): %d\n", totalCost);
    printf("  =============================================\n");
}

//collects matching nodes into array, then prints with header //

void collectRange(TripNode *root, int d1, int d2,
                  TripNode *arr[], int *size){
    if(root == NULL) return;

    // AVL pruning //
    if(root->date > d1)
        collectRange(root->left,  d1, d2, arr, size);

    if(root->date >= d1 && root->date <= d2)
        arr[(*size)++] = root;

    if(root->date < d2)
        collectRange(root->right, d1, d2, arr, size);
}

void rangeSearch(TripList *head){
    if(head == NULL){ printf("  No trips available!\n"); return; }

    int d1, d2;
    printf("  Enter Start Date (YYYYMMDD): ");
    scanf("%d", &d1);
    printf("  Enter End   Date (YYYYMMDD): ");
    scanf("%d", &d2);

    if(d1 > d2){
        printf("  Error: Start date cannot be after end date!\n");
        return;
    }

    printf("\n  =====================================================\n");
    printf("       RANGE SEARCH  :  %d  to  %d\n", d1, d2);
    printf("  =====================================================\n");

    int grandTotal   = 0;
    TripList *curr   = head;

    while(curr != NULL){
        TripNode *arr[200];
        int size = 0;

        collectRange(curr->actRoot, d1, d2, arr, &size);

        if(size > 0){
            printf("\n  +-------------------------------------------------+\n");
            printf("  | Trip [%d] : %-37s|\n", curr->tripId, curr->tripName);
            printf("  | Route    : %-20s -> %-13s|\n",
                   curr->source, curr->destination);
            printf("  | Activities in range : %-25d|\n", size);
            printf("  +-------------------------------------------------+\n");

            for(int i = 0; i < size; i++){
                TripNode *node = arr[i];
                printf("\n    [%d] %-10s  @ %-20s  Date: %d\n",
                       i + 1,
                       node->activityType,
                       node->location,
                       node->date);
                printf("        Cost: %-8d  Skipped: %s\n",
                       node->cost,
                       node->skipped ? "Yes" : "No");

                if(node->navRoot != NULL){
                    printf("        -- Navigation --\n");
                    displayNavInorder(node->navRoot);
                } else {
                    printf("        (No navigation steps)\n");
                }
            }
            grandTotal += size;
        }

        curr = curr->next;
    }

    if(grandTotal == 0)
        printf("\n  No activities found between %d and %d.\n", d1, d2);
    else{
        printf("\n  =====================================================\n");
        printf("  Total activities found : %d\n", grandTotal);
        printf("  =====================================================\n");
    }
}

/* ============================================================
   FILE HANDLING
   ============================================================ */

// ---- SAVE ---- //

void saveNavToFile(FILE *fp, NavNode *root){
    if(root == NULL) return;
    saveNavToFile(fp, root->left);
    fprintf(fp, "NAV|%d|%s|%d|%d\n",
            root->stepId, root->directions, root->distance, root->cost);
    saveNavToFile(fp, root->right);
}

void saveTripNodesToFile(FILE *fp, TripNode *root){
    if(root == NULL) return;
    saveTripNodesToFile(fp, root->left);
    fprintf(fp, "ACT|%d|%s|%s|%d|%d|%d\n",
            root->actId, root->activityType, root->location,
            root->date, root->cost, root->skipped);
    saveNavToFile(fp, root->navRoot);
    fprintf(fp, "ENDACT\n");
    saveTripNodesToFile(fp, root->right);
}

void saveToFile(TripList *head){
    char filename[100];
    printf("  Enter filename to save (e.g. trips.txt): ");
    scanf(" %[^\n]", filename);

    FILE *fp = fopen(filename, "w");
    if(!fp){ printf("  Error: Cannot create file '%s'!\n", filename); return; }

    TripList *curr = head;
    while(curr){
        curr->totalCost = calculateTotalCost(curr->actRoot);   // refresh cost //
        fprintf(fp, "TRIP|%d|%s|%s|%s|%d\n",
                curr->tripId, curr->tripName,
                curr->source, curr->destination, curr->totalCost);
        saveTripNodesToFile(fp, curr->actRoot);
        fprintf(fp, "ENDTRIP\n");
        curr = curr->next;
    }

    fclose(fp);
    printf("  Data saved to '%s' successfully!\n", filename);
}

/* ---- LOAD ---- */

TripList* loadFromFile(TripList *head){
    char filename[100];
    printf("  Enter filename to load (e.g. trips.txt): ");
    scanf(" %[^\n]", filename);

    FILE *fp = fopen(filename, "r");
    if(!fp){ printf("  Error: File '%s' not found!\n", filename); return head; }

    char line[400];
    TripList *currentTrip = NULL;
    TripNode *currentAct  = NULL;

    while(fgets(line, sizeof(line), fp)){
        line[strcspn(line, "\n")] = 0;   // strip newline //

        if(strncmp(line, "TRIP|", 5) == 0){
            currentTrip = (TripList*)malloc(sizeof(TripList));
            currentTrip->actRoot = NULL;
            currentTrip->next    = NULL;

            // Format: TRIP|id|name|source|destination|totalCost //
            sscanf(line + 5, "%d|%99[^|]|%49[^|]|%49[^|]|%d",
                   &currentTrip->tripId,   currentTrip->tripName,
                   currentTrip->source,    currentTrip->destination,
                   &currentTrip->totalCost);

            // Skip duplicate trip IDs //
            if(findTripId(head, currentTrip->tripId)){
                printf("  Warning: Trip ID %d already loaded. Skipping.\n",
                       currentTrip->tripId);
                free(currentTrip); currentTrip = NULL; continue;
            }

            // Append to list //
            if(head == NULL){ head = currentTrip; }
            else{
                TripList *curr = head;
                while(curr->next) curr = curr->next;
                curr->next = currentTrip;
            }
        }
        else if(strncmp(line, "ACT|", 4) == 0 && currentTrip != NULL){
            currentAct = (TripNode*)malloc(sizeof(TripNode));
            currentAct->navRoot = NULL;
            currentAct->left = currentAct->right = NULL;
            currentAct->height = 1;

            // Format: ACT|id|type|location|date|cost|skipped //
            sscanf(line + 4, "%d|%99[^|]|%99[^|]|%d|%d|%d",
                   &currentAct->actId,  currentAct->activityType,
                   currentAct->location, &currentAct->date,
                   &currentAct->cost,    &currentAct->skipped);

            currentTrip->actRoot = insertTripHelper(currentTrip->actRoot, currentAct);
        }
        else if(strncmp(line, "NAV|", 4) == 0 && currentAct != NULL){
            NavNode *nav = (NavNode*)malloc(sizeof(NavNode));
            nav->left = nav->right = NULL;
            nav->height = 1;

            // Format: NAV|id|directions|distance|cost //
            sscanf(line + 4, "%d|%49[^|]|%d|%d",
                   &nav->stepId, nav->directions,
                   &nav->distance, &nav->cost);

            currentAct->navRoot = insertNavHelper(currentAct->navRoot, nav);
        }
        else if(strcmp(line, "ENDACT") == 0){ currentAct  = NULL; }
        else if(strcmp(line, "ENDTRIP") == 0){ currentTrip = NULL; }
    }

    fclose(fp);
    printf("  Data loaded from '%s' successfully!\n", filename);
    return head;
}


/* ============================================================
   MENUS
   ============================================================ */

void navMenu(TripNode *act){
    int choice;
    do{
        printf("\n     ----- Navigation Menu  [Activity %d] -----\n", act->actId);
        printf("     1. Add Navigation Step\n");
        printf("     2. Delete Navigation Step\n");
        printf("     3. Update Navigation Step\n");
        printf("     4. Display All Nav Steps\n");
        printf("     5. Search Direction Between Steps\n");
        printf("     0. Back\n");
        printf("     Choice: ");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                act->navRoot = insertNavNode(act->navRoot);
                break;
            case 2:{
                int id;
                printf("  Enter Step ID to delete: ");
                scanf("%d", &id);
                if(!FindNavNode(act->navRoot, id))
                    printf("  Step not found!\n");
                else{
                    act->navRoot = deleteNavNode(act->navRoot, id);
                    printf("  Step deleted!\n");
                }
                break;
            }
            case 3:{
                int id;
                printf("  Enter Step ID to update: ");
                scanf("%d", &id);
                act->navRoot = updateNavNode(act->navRoot, id);
                break;
            }
            case 4:
                if(act->navRoot == NULL) printf("  No steps yet.\n");
                else{ printf("  --- Navigation Steps ---\n"); displayNavInorder(act->navRoot); }
                break;
            case 5:
                searchDirectionBetween(act->navRoot);
                break;
            case 0: break;
            default: printf("  Invalid choice!\n");
        }
    } while(choice != 0);
}

void tripMenu(TripList *trip){
    int choice;
    do{
        trip->totalCost = calculateTotalCost(trip->actRoot);   // keep cost live //

        printf("\n  ============  %s  [%s -> %s]  ============\n",
               trip->tripName, trip->source, trip->destination);
        printf("  Total Cost : %d\n", trip->totalCost);
        printf("  -------------------------------------------\n");
        printf("   1. Add Activity\n");
        printf("   2. Delete Activity\n");
        printf("   3. Display All Activities\n");
        printf("   4. Update Activity Details\n");
        printf("   5. Manage Navigation (for an activity)\n");
        printf("   6. Detect Duplicate Locations\n");
        printf("   7. Sort Hotels by Cost\n");
        
        printf("   8. Calculate & Show Total Cost\n");
        printf("   9. Search path source to destination\n");
        printf("   0. Back to Main Menu\n");
        printf("  Choice: ");
        scanf("%d", &choice);

        switch(choice){
            case 1:
                trip->actRoot   = insertTripNode(trip->actRoot);
                trip->totalCost = calculateTotalCost(trip->actRoot);
                break;

            case 2:{
                int id;
                printf("  Enter Activity ID to delete: ");
                scanf("%d", &id);
                if(!FindTripNode(trip->actRoot, id))
                    printf("  Activity not found!\n");
                else{
                    trip->actRoot   = deleteTripNode(trip->actRoot, id);
                    trip->totalCost = calculateTotalCost(trip->actRoot);
                    printf("  Activity deleted!\n");
                }
                break;
            }

            case 3:
                displayTripInorder(trip->actRoot);
                break;

            case 4:
                updateTripActivity(trip->actRoot);
                trip->totalCost = calculateTotalCost(trip->actRoot);
                break;

            case 5:{
                int id;
                printf("  Enter Activity ID to manage navigation: ");
                scanf("%d", &id);
                TripNode *act = getTripNode(trip->actRoot, id);
                if(act == NULL) printf("  Activity not found!\n");
                else            navMenu(act);
                trip->totalCost = calculateTotalCost(trip->actRoot);
                break;
            }

            case 6:
                detectDuplicateLocations(trip->actRoot);
                break;

            case 7:
                sortHotelCosts(trip->actRoot);
                break;

            case 8:{
                int total = calculateTotalCost(trip->actRoot);
                trip->totalCost = total;
                printf("\n  Total Cost for '%s': %d\n", trip->tripName, total);
                break;
            }

            case 9:  // shift your old case 8 to case 9 //
                findPath(trip->actRoot);
                break;

            case 0:
                printf("  Returning to main menu...\n");
                break;

            default:
                printf("  Invalid choice!\n");
        }
    } while(choice != 0);
}


/* ============================================================
   MAIN
   ============================================================ */

int main(){
    TripList *head = NULL;
    int choice;

    printf("\n");
    printf("  =============================================\n");
    printf("        TRIP PLANNER  -  AVL TREE EDITION      \n");
    printf("  =============================================\n");

    do{
        printf("\n  ============  MAIN MENU  ============\n");
        printf("   1. Add New Trip\n");
        printf("   2. View All Trips\n");
        printf("   3. Select & Manage a Trip\n");
        printf("   4. Delete a Trip\n");
        printf("   5. Save All Data to File\n");
        printf("   6. Load Data from File\n");
        printf("   7. Range Search (by Date)\n");
        printf("   0. Exit\n");
        printf("  =====================================\n");
        printf("  Choice: ");
        scanf("%d", &choice);

        switch(choice){

            case 1:
                head = addTrip(head);
                printf("  Trip added successfully!\n");
                break;

            case 2:
                displayAllTrips(head);
                break;

            case 3:{
                if(head == NULL){ printf("  No trips! Add one first.\n"); break; }
                displayAllTrips(head);
                int id;
                printf("  Enter Trip ID to manage: ");
                scanf("%d", &id);
                TripList *trip = getTrip(head, id);
                if(trip == NULL) printf("  Trip not found!\n");
                else             tripMenu(trip);
                break;
            }

            case 4:{
                if(head == NULL){ printf("  No trips to delete!\n"); break; }
                displayAllTrips(head);
                int id;
                printf("  Enter Trip ID to delete: ");
                scanf("%d", &id);
                head = deleteTrip(head, id);
                break;
            }

            case 5:
                if(head == NULL) printf("  No data to save!\n");
                else             saveToFile(head);
                break;

            case 6:
                head = loadFromFile(head);
                break;

            case 7:    
                rangeSearch(head);
                break;
            case 0:
                // if(head == NULL) printf("  No data to save!\n");
                // else             saveToFile(head);
                printf("\n  Thank you for using Trip Planner! Goodbye!\n\n");
                break;

            default:
                printf("  Invalid choice! Try again.\n");
        }

    } while(choice != 0);

    // Free all memory //
    TripList *curr = head;
    while(curr){
        TripList *tmp = curr->next;
        freeTripTree(curr->actRoot);
        free(curr);
        curr = tmp;
    }

    return 0;
}
