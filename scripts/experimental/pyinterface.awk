
awk -F : '					\
BEGIN{i=2;}
{
    if ($1 == "%%N") AppName=$1;
    if ($1=="%%P")
    {
	name=$2
	def=$4
	type=$3

	if (type ~ "\[")
	{
	    ob="[";
	    cb="]";
	}
	else {ob="";cb="";}

	if (i>NR) delim=""
	else delim=","
	if (type=="string")
	    printf(" \"%s\"_a=%s\"%s\"%s%s",name,ob,def,cb,delim);
	else
	    if (type=="bool")
	    {
		if (def==1)
		    printf(" \"%s\"_a=%strue%s%s",name,ob,cb,delim);
		else
		    printf(" \"%s\"_a=%sfalse%s%s",name,ob,cb,delim);
	    }
	    else printf(" \"%s\"_a=%s%s%s%s",name,ob,def,cb,delim);

	i=i+1;
    }
}
END{printf("\n");}
' $1
