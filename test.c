int a;

int add(int x,int y)
{
	return x+y;
}

int main()
{
	int a,b,c,i;
	a = 2;
	b = 1;
	c = 0;
	for(i=0;i<5;i++){
		b = add(a--,b);
	}
	if(c==b){
		c = 5;
	}	
	return c;
}
