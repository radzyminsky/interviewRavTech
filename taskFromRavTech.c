#include <stdio.h>
#include <time.h>
#include <string.h>

#pragma warning(disable : 4996)

struct Line_
{
	char id[20];
	char fname[20];
	char lname[20];
	char fon_number[11];

	struct tm date;
	int debt;
	struct Line_* next;

	char err_msg[100];
};
typedef struct Line_ Line;

typedef int (*fun_ptr)(Line*, char*);
void addToSumList(Line* lin);
void init_fname_lname(Line* l, char* temp, int num_field) {
	char err_msg[30];
	char* field;
	switch (num_field)
	{
	case 1:
		strcpy(err_msg, "error in fname field, ");
		field = l->fname;
		break;
	case 2:
		strcpy(err_msg, "error in lname field, ");
		field = l->lname;
		break;
	default:
		field = NULL;
		break;
	}
	for (int i = 0;i < strlen(temp);i++) {
		if (temp[i] < 'A' || temp[i]>'z') {
			strcat(l->err_msg, err_msg);
			break;
		}
	}
	if (!strlen(temp))
		strcat(l->err_msg, err_msg);
	strcpy(field, temp);
}

void init_id_fon(Line* l, char* temp, int num_field) {
	char err_msg[30];
	char* field;
	switch (num_field)
	{
	case 1:
		strcpy(err_msg, "error in id field, ");
		field = l->id;
		break;
	case 2:
		strcpy(err_msg, "error in fon_number field, ");
		field = l->fon_number;
		break;
	default:
		field = NULL;
		break;
	}
	for (int i = 0;i < strlen(temp);i++) {
		if (temp[i] < '0' || temp[i]>'9') {
			strcat(l->err_msg, err_msg);
			break;
		}

	}
	if (!strlen(temp))
		strcat(l->err_msg, err_msg);
	strcpy(field, temp);

}

struct tm* str_to_date(char* arg) {

	int dd_mm_yy[] = { 0,0,0 }, year;
	int i, j = 0, its_err = 0;
	char* str, string[50], delim[] = "./\\";

	strcpy(string, arg);
	str = strtok(string, delim);

	for (j = 0; j < 3; j++)
	{
		for (i = 0; i < strlen(str); i++)
		{
			if (str[i] < '0' || str[i]>'9')
				its_err = 1;
		}
		if (!its_err)
			sscanf(str, "%d", dd_mm_yy + j);
		str = strtok(NULL, delim);
	}

	if (str)	//that is the date has mor than "dd/mm/yy"
		its_err = 1;
	if (its_err) {
		return NULL;
	}
	if (dd_mm_yy[2] < 100)
		year = dd_mm_yy[2];
	else
		year = dd_mm_yy[2] - 1900;
	struct tm date = { 0, 0, 0, dd_mm_yy[1], dd_mm_yy[0] - 1,year, 0, 0, 0 };
	return &date;

}

void init_date(Line* l, char* arg) {
	struct tm* date = str_to_date(arg);
	if (date)
		l->date = *date;
	else {
		struct tm d = { 0, 0, 0, 1,1,1, 0, 0, 0 };
		l->date = d;
		strcat(l->err_msg, "error in date field, ");
	}
}

void init_debt(Line* l, char* temp) {
	int debt;
	sscanf(temp, "%d", &debt);
	if (debt > 0) {
		strcat(l->err_msg, "error in debt field, ");
		debt = 0;
	}
	l->debt = debt;
}

void freeList(Line* l) {
	if (!l)
		return;
	freeList(l->next);
	free(l);
}

void insertValues(char* line, Line* new_line) {
	int j;
	char* token, * token2 = "", delimit[] = ",\n";

	token = strtok(line, delimit);

	for (j = 0; j < 6; j++)
	{

		switch (j)
		{
		case 0:
			init_id_fon(new_line, token, 1);
			token = strtok(NULL, delimit);
			break;
		case 1:
			init_fname_lname(new_line, token, 1);
			token = strtok(NULL, delimit);
			break;
		case 2:
			init_fname_lname(new_line, token, 2);
			token = strtok(NULL, delimit);
			break;
		case 3:
			init_id_fon(new_line, token, 2);
			token = strtok(NULL, delimit);
			break;
		case 4:
			token2 = strtok(NULL, delimit);//notice, that is a "token2"
			init_date(new_line, token);
			break;
		case 5:
			init_debt(new_line, token2);
			break;
		default:
			break;
		}
	}
}

Line* createNewLine(char* string) {
	Line* new_line;
	//create new item in list
	new_line = (Line*)malloc(sizeof(Line));
	if (new_line) {
		new_line->err_msg[0] = 0;

		//insert values to a new item
		if (string)
			insertValues(string, new_line);
	}
	return new_line;
}

void sort(Line** point_to_this_link) {

	Line* this_link = *point_to_this_link;
	while (this_link->next && (this_link->next->debt > this_link->debt))
	{
		*point_to_this_link = this_link->next;
		this_link->next = (*point_to_this_link)->next;
		(*point_to_this_link)->next = this_link;
		point_to_this_link = &((*point_to_this_link)->next);
	}
}

int addNewLine(char* string) {
	extern Line* head;
	Line* new_line = createNewLine(string);	//create new item in list

	if (!new_line)
		return 0;

	new_line->next = head;
	head = new_line;
	sort(&head);
	addToSumList(new_line);
	return 1;
}

void set(char* str) {
	addNewLine(str + 4);
}

void printOne(Line* l) {
	char date[20];
	strftime(date, 20, "%x", &l->date);
	printf("%s, %s, %s,  %s, %s, %d\nerror msg: %s\n",
		l->id, l->fname, l->lname, l->fon_number, date, l->debt, l->err_msg);
}

void printList(Line* temp_line) {

	while (temp_line)
	{
		printOne(temp_line);
		temp_line = temp_line->next;
	}
}

void search(fun_ptr fun, Line* l, char* c) {
	while (l)
	{
		if (fun(l, c))
			printOne(l);
		l = l->next;
	}
}

int id_equ(Line* l, char* id) {
	int n = !strcmp(l->id, id);
	return n;
}
int fname_equ(Line* l, char* fname) {
	return (!strcmp(l->fname, fname));
}
int lname_equ(Line* l, char* lname) {
	return (!strcmp(l->lname, lname));
}
int fon_equ(Line* l, char* fon_number) {
	return (!strcmp(l->fon_number, fon_number));
}
int date_equ(Line* l, char* date) {
	struct tm* d = str_to_date(date);
	if (d) {
		/*time_t t = mktime(&(*d));
		int n= !difftime(mktime(&l->date), t);*/
		return(l->date.tm_year == d->tm_year &&
			l->date.tm_mon == d->tm_mon &&
			l->date.tm_mday == d->tm_mday);
	}
	return 0;
}
int debt_equ(Line* l, char* debt) {
	int debt_;
	sscanf(debt, "%d", &debt_);
	return l->debt == debt_;
}
int date_larger2(struct tm* d1, struct tm* d2) {
	int n;
	n = (d1->tm_year > d2->tm_year ||
		d1->tm_year == d2->tm_year && (d1->tm_mon > d2->tm_mon ||
			d1->tm_mon == d2->tm_mon && d1->tm_mday > d2->tm_mday));
	return n;
}
int date_larger(Line* l, char* date) {
	struct tm* d = str_to_date(date);
	int n;
	if (d) {
		n = date_larger2(&l->date, d);
		return n;
	}
	return 0;
}
int debt_larger(Line* l, char* debt) {
	int debt_;
	sscanf(debt, "%d", &debt_);
	return l->debt > debt_;
}

int date_smaller(Line* l, char* date) {
	return !(date_equ(l, date) || date_larger(l, date));
}
int debt_smaller(Line* l, char* debt) {
	int debt_;
	sscanf(debt, "%d", &debt_);
	return l->debt < debt_;
}


fun_ptr command_equal(char* str1, char* str2) {
	fun_ptr fun = NULL;
	if (!strcmp(str1, "id")) {
		fun = &id_equ;
	}
	else if (!strcmp(str1, "fname"))
	{
		fun = &fname_equ;
	}
	else if (!strcmp(str1, "lname"))
	{
		fun = &lname_equ;
	}
	else if (!strcmp(str1, "fon"))
	{
		fun = &fon_equ;
	}
	else if (!strcmp(str1, "date"))
	{
		fun = date_equ;
	}
	else if (!strcmp(str1, "debt"))
	{
		fun = debt_equ;
	}
	return fun;
}
fun_ptr command_larger(char* str1, char* str2) {
	fun_ptr fun = NULL;

	if (!strcmp(str1, "date"))
		fun = &date_larger;

	else if (!strcmp(str1, "debt"))
		fun = &debt_larger;

	return fun;
}
fun_ptr command_smaller(char* str1, char* str2) {
	fun_ptr fun = NULL;

	if (!strcmp(str1, "date"))
		fun = &date_smaller;

	else if (!strcmp(str1, "debt"))
		fun = &debt_smaller;

	return fun;
}

void commands() {
	extern Line* head, * head_sumList;
	fun_ptr fun = NULL;
	char str[100] = "", * c;


	while (strncmp(str, "qite\n", 5)) {
		/// </summary>
		printList(head);
		printf("\n---sum list---\n");
		printList(head_sumList);
		///
		printf(">>\n");
		fgets(str, 100, stdin);

		if (!strncmp(str, "set ", 4)) {
			set(str);
			continue;
		}
		else if (c = strchr(str, '=')) {
			fun = command_equal(strtok(str, "="), strtok(c + 1, "\n"));
		}
		else if (c = strchr(str, '<')) {
			fun = command_smaller(strtok(str, "<"), strtok(c + 1, "\n"));
		}
		else if (c = strchr(str, '>')) {
			fun = command_larger(strtok(str, ">"), strtok(c + 1, "\n"));
		}
		if (!fun) {
			printf("\"%s\": isn't command, error..\n", strtok(str, "\n"));
			continue;
		}
		if (!strcmp(str, "date"))
			search(fun, head, c + 1);
		else
			search(fun, head_sumList, c + 1);

	}
}

void updateSumList(Line* this_line, Line* lin) {

	this_line->debt += lin->debt;
	if (date_larger2(&this_line->date, &lin->date))
		this_line->date = lin->date;
	sort(&this_line);
}

void addToSumList(Line* lin) {
	extern Line* head_sumList;

	Line* newL = createNewLine(NULL);
	if (!newL)
		return;
	*newL = *lin;
	Line* this_line = head_sumList;
	while (this_line) {
		if (!strcmp(this_line->id, newL->id)) {
			updateSumList(this_line, newL);
			return;
		}
		this_line = this_line->next;
	}
	newL->next = head_sumList;
	head_sumList = newL;
	sort(&head_sumList);
}

//global vuriable to the lists
Line* head = NULL, * head_sumList = NULL;

int main()
{
	char string[200];
	int sucsses = 0;

	FILE* file = fopen("taskRavTech.csv", "a+t");

	if (!file)
		return -1;


	//ignore from high line
	while (fgetc(file) != '\n') {};

	while (fgets(string, 200, file))
	{
		//create new item in list
		sucsses = addNewLine(string);
		if (!sucsses)
			return -1;
	}

	fclose(file);

	//print all list
	printList(head);
	printf("\n---sum list---\n");
	printList(head_sumList);

	commands();

	freeList(head);
	freeList(head_sumList);

	return 0;
}
