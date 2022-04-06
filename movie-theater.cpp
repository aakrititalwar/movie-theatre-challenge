#include <iostream>
#include <fstream>
#include <string>
#include <getopt.h>
#include <cstring>
#include <cctype>
#include <string>
#include <bits/stdc++.h>
#include <queue>
using namespace std;

int total_rows = 10;
int total_cols = 20;

map<string,vector<tuple<char,int,int>>> seat_alloc; // seat_alloc [map]  ==>   reserve_id ->  (row_name, starting seat, tickets alloted)

map<int,deque<pair<int,int>>> curr_free;      // stores free groups of each row in desc order of size
                                              // curr_free [map] ==>  row_num  ->  [(group_size, align), (group_size, align)]
int curr_avail_seats = total_cols * total_rows;

static char linebuf[1024]; 
FILE* inpfile;                                // for getting input from file
const char* DELIM = " \t\n\r\v\f";

auto comp =[](pair<int,int> a, pair<int,int> b){       // comparator class for max_groups priority queue
    //comparison logic
    if(a.first>b.first)
        return false;
    else if(a.first<b.first)
        return true;
    else{ //when hole size is same
    if (a.second<b.second)
        return false;
    else
        return true;
    }
};

priority_queue<pair<int,int>, vector<pair<int,int>>, decltype(comp)> max_groups(comp);     // priority_queue for all groups available in all rows

void free_group_init(){                       // when all rows are empty
    for(int i = 0; i < total_rows; i++){
        deque<pair<int,int>> q;
        q.push_back(make_pair(total_cols,0));
        curr_free[i] = q;
    }
}

string convertToString(char* a)             // convert to String
{   
    int size = strlen(a);
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

char get_alpha(int rownum){               // get alphabet of row from rownum   J-A  => 0-9
    return ('J' - rownum);
}

 void initialise_max_groups(){
     while(!max_groups.empty()){         // clear priority_queue 
         max_groups.pop();
     }
     for(int i = 0 ; i < total_rows; i++){                     // add all groups to priority queue in the form (group_size, row_num)
         for(int j = 0 ; j < curr_free[i].size(); j++){
             pair<int,int> p2 = make_pair(curr_free[i][j].first,i);
             max_groups.push(p2);
         }     
     }
 }
void push_to_deq( int row, pair<int,int> p){
    if(curr_free[row].empty()){
        curr_free[row].push_back(p);
    }
    else if(curr_free[row][0].first<=p.first){  // pair with max size goes first.
        curr_free[row].push_front(p);
    }
    else {
        curr_free[row].push_back(p);
    }
    return;
}

void scattered_alloc(string reserve_id, int num_ticket){      // scattered Allocation
    if(num_ticket == 0){
        return;
    }
    pair<int,int> max_pair = max_groups.top();               // pair for largest group in all rows
    max_groups.pop();
    int max_row = max_pair.second;                           // row with largest group
    int max_hole_size = max_pair.first;                      // largest group size
    tuple<char,int,int> tup;
        if(max_hole_size<= num_ticket){     // if number of tickets is more than the max available group size

                if(curr_free[max_row][0].second == 1){                                                                            // if hole is right aligned
                    tup = make_tuple(get_alpha(max_row),total_cols-max_hole_size+1,max_hole_size);
                }
                else{                                                                                                      // if either row is empty or hole is left aligned
                    tup = make_tuple(get_alpha(max_row),1,max_hole_size);
                }
                curr_free[max_row].pop_front();
                if(seat_alloc[reserve_id].empty()){                           // allocating all seats in the group
                    vector<tuple<char,int,int>> v;
                    seat_alloc[reserve_id] = v;
                }
                seat_alloc[reserve_id].push_back(tup);
                curr_avail_seats -= max_hole_size;
                scattered_alloc(reserve_id,num_ticket - max_hole_size);   // recursivly allocating remaining tickets 
        }
        else{    
                                                   // when available hole size is more than tickets
                if(curr_free[max_row][0].second == -1){     // if left aligned hole
                curr_free[max_row].pop_front();
                tup = make_tuple(get_alpha(max_row),total_cols-max_hole_size+1,num_ticket);
                push_to_deq(max_row,make_pair((max_hole_size - num_ticket),-1));
                }
                else if(curr_free[max_row][0].second == 0){   // if row is empty
                curr_free[max_row].pop_front();
                int rem_seats = total_cols - num_ticket;
                int st_seat = (int)(rem_seats/2) + 1;
                if(st_seat-1 != 0) {
                    push_to_deq(max_row,make_pair(st_seat-1,-1));
                }
                if((rem_seats-(st_seat-1)) != 0) {
                    push_to_deq(max_row, make_pair((rem_seats-(st_seat-1)),1));
                }   
                tup = make_tuple(get_alpha(max_row),st_seat,num_ticket);
                }
                else{                                  // if hole is right aligned
                    curr_free[max_row].pop_front();
                    tup = make_tuple(get_alpha(max_row),max_hole_size+1,num_ticket);
                    push_to_deq(max_row,make_pair((max_hole_size - num_ticket),1));
                }
                if(seat_alloc[reserve_id].empty()){     //allocating seats
                    vector<tuple<char,int,int>> v;
                    seat_alloc[reserve_id] = v;
                }
                curr_avail_seats -= num_ticket;
                seat_alloc[reserve_id].push_back(tup);
        }

}

void cont_allocation(string reserve_id, int num_ticket){
 
 
 if(curr_avail_seats == 0) return;             // if no seats available return

 bool allocated = false;

 for(int i = 0 ; i < total_rows; i++){
     if(curr_free[i].empty()){            //if row completely filled move on to next
         continue;
     }
     pair<int,int> row_max_avail = curr_free[i][0];  // hole with max size
      
     if(row_max_avail.first>=num_ticket){       
         int st_seat;
         curr_free[i].pop_front();

         if(row_max_avail.second == 0){     // if row is completely empty 
             int rem_seats = total_cols - num_ticket;
             st_seat = (int)(rem_seats/2) + 1;
             if(st_seat-1 != 0) {
                 push_to_deq(i,make_pair(st_seat-1,-1));
             }
             if((rem_seats-(st_seat-1)) != 0) {
                 push_to_deq(i, make_pair((rem_seats-(st_seat-1)),1));
             }
         }

         else if(row_max_avail.second == -1){                 // if max hole in row is on the left side
             int rem_seats = row_max_avail.first - num_ticket; 
             st_seat = rem_seats+1;
             if(rem_seats != 0){
               push_to_deq(i, make_pair(rem_seats,-1));
             }
         }

         else{                                                   // if max hole in row is on the right side
             int rem_seats = row_max_avail.first - num_ticket;
             st_seat = (total_cols - row_max_avail.first) + 1;
             if(rem_seats != 0){
             push_to_deq(i,make_pair(rem_seats,1));
             }
         }
         allocated = true;
         vector<tuple<char,int,int>> v;           //    value of seat_alloc map is vector of tuples to cater to scattered allocation
         tuple<char,int,int> t = make_tuple(get_alpha(i),st_seat,num_ticket);
         v.push_back(t);
         seat_alloc.insert(make_pair(reserve_id,v));  // allocating seats
         curr_avail_seats -= num_ticket;                         // updating current available seats
         break;
     }
 }
 if(!allocated){                               // if still not allocated 
    if(curr_avail_seats>=num_ticket){          // and number of seats available
       initialise_max_groups();   
       scattered_alloc(reserve_id,num_ticket);            // calling for scattered allocation
    }
 return;    
}

}
void read_inp_file(){
   while(fgets(linebuf,1024, inpfile)){
       char* res_id = strtok(linebuf, DELIM);
       string reserve_id = convertToString(res_id);
       char* num = strtok(NULL, DELIM);
       int num_ticket = atoi(num);  
       cont_allocation(reserve_id, num_ticket);
   }
}

void print_arr(){
    auto it = seat_alloc.begin();
    while(it != seat_alloc.end()){
        vector<tuple<char,int,int>> v = (it->second);
        //int num = get<2>(tup);
        cout << endl;
        cout << it->first;
        int idx = 0;
        while(idx < v.size()){
            int num = get<2>(v[idx]);
            while(num){
                cout << " " << get<0>(v[idx]) << (get<1>(v[idx]) + (get<2>(v[idx]) - num));
                num--;
            }
         idx++;
        }
        it++;
    }
}

void gen_output(){
    auto it = seat_alloc.begin();
    ofstream out_file ("mvout.txt");
    if(out_file.is_open()){
    while(it != seat_alloc.end()){
        string line_str = "";
        vector<tuple<char,int,int>> v = (it->second);
        line_str += it->first;
        int idx = 0;
        while(idx < v.size()){
            int num = get<2>(v[idx]);
            while(num){
                line_str = line_str + " " + get<0>(v[idx]) + to_string(get<1>(v[idx]) + (get<2>(v[idx]) - num));
                num--;
            }
         idx++;
        }
        line_str += "\n";
        out_file << line_str;
        it++;
    }
    out_file.close();
    }
}


int main(int argc, char* argv[]){
    free_group_init();    // initialise current rows with free froup size of total_cols
    inpfile = fopen(argv[1], "r");    
    read_inp_file();                       // reading input file
    print_arr();
    gen_output();
    return 0; 
}