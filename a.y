%{
	/**test**/


%}
%token	add or b zk yk not
%type	<name>	C
%left	C

%%
E	:E add E
	;
E	:E or E
	;
E	:b
	;
E	:not E
	;
E	:zk E yk
	;
%%
int main(){
	/***************/
	return 0;
}