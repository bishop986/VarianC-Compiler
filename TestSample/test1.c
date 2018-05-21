int f() {          
	return 200;
}

int i = 1;

int main() {    
	int main;
	main = f();
	putIntLn(i);
	{
		int i = 2;
		int main;   
		int f;     
		main = f = 100;
		putIntLn(i);
		putIntLn(main);
		putIntLn(f);
	}
	putIntLn(main);
}
