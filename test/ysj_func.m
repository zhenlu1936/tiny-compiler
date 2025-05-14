int max(int x,int y)
{
	int z;
	if(x>y){
		z=x;
	}
	else{z=y;}
	return z;
}

int main() 
{
	int a,b,c;
	input a;
	input b;
	c = max(a,b);
	output c;
	output "\n";
	return 0;
}