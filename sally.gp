\g 1;
\gm 2;

install(Fp_ellcard_SEA,"GGGL");
allocatemem(4000000000);
default(nbthreads,8);

Sal224 = 2^224 - 2^96 + 1
Sal256 = 2^256 - 2^224 + 2^192 + 2^96 - 1
Sal384 = 2^384 - 2^128 - 2^96 + 2^32 - 1
Sal521 = 2^521 - 1

seabyj2(j0) = {
 my(n,c,p,cofactor);
 n = 384;
 c = "sal_";
 p =  2^384 - 2^128 - 2^96 + 2^32 - 1;
 cofactor = 1;
 my(filename=Str("j", n, "_", c, "/", j0));
 my(E,tff,tf,q,qt,pq,pt,ff,r,a4,a6);
 if(j0 == 1728 || j0 == 0, return());
 print("j0 = ", j0, "\n");
 a4 = (36 / (j0 - 1728)) % p;
 a6 = (-1 / (j0 - 1728)) % p;
 /*print("a4 = ", a4, ", a6 = ", a6, "\n");*/
 tff = Fp_ellcard_SEA(a4, a6, p, cofactor);
 print("tff = ", tff, "\n"); 
 if(tff[1] != 0,  return());
 tf = tff[2];
 q  = p + 1 - tf;
 qt = p + 1 + tf;
 pq = isprime(q);
 pt = isprime(qt);
 if((pq||pt),
    write(filename, [[pq&&pt, [pq, pt]], tff, [factor(q, 2^16), factor(qt, 2^16)]]);
    return() 
   );
};
my(sea=seabyj2); r = parfor(i=0, 2^20, sea(i));
