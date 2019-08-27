// Kinematics code from here! http://www.starlino.com/dcm_tutorial.html
#ifndef DCM__H
#define DCM__H
#include <math.h>
//get modulus of a 3d vector sqrt(x^2+y^2+y^2)
float vector3d_modulus(float* vector){
	static float R;  
	R = vector[0]*vector[0];
	R += vector[1]*vector[1];
	R += vector[2]*vector[2];
	return sqrt(R);
}

//convert vector to a vector with same direction and modulus 1
void vector3d_normalize(float* vector){
	static float R;  
	R = vector3d_modulus(vector);
	vector[0] /= R;
	vector[1] /= R; 
	vector[2] /= R;  
}

//calcuate vector dot-product  c = a . b
float vector3d_dot(float* a,float* b){
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}


//calcuate vector cross-product  c = a x b
void vector3d_cross(float* a,float* b, float* c){
	c[0] = a[1]*b[2] - a[2]*b[1];
	c[1] = a[2]*b[0] - a[0]*b[2];
	c[2] = a[0]*b[1] - a[1]*b[0];
}

//calcuate vector scalar-product  n = s x a
void vector3d_scale(float s, float* a , float* b){
	b[0] = s*a[0];
	b[1] = s*a[1];
	b[2] = s*a[2];
} 


//calcuate vector sum   c = a + b
void vector3d_add(float* a , float* b, float* c){
	c[0] = a[0] + b[0];
	c[1] = a[1] + b[1];
	c[2] = a[2] + b[2];
} 


//creates equivalent skew symetric matrix plus identity
//for v = {x,y,z} returns
// m = {{1,-z,y}
//		{z,1,-x}
//		{-y,x,1}}
void vector3d_skew_plus_identity(float *v,float* m){
	m[0*3+0]=1;
	m[0*3+1]=-v[2];
	m[0*3+2]=v[1];
	m[1*3+0]=v[2];
	m[1*3+1]=1;
	m[1*3+2]=-v[0];
	m[2*3+0]=-v[1];
	m[2*3+1]=v[0];
	m[2*3+2]=1;
}

//bring dcm matrix in order - adjust values to make orthonormal (or at least closer to orthonormal)
void dcm_orthonormalize(float dcm[3][3]){
	//err = X . Y ,  X = X - err/2 * Y , Y = Y - err/2 * X  (DCMDraft2 Eqn.19)
	float err = vector3d_dot((float*)(dcm[0]),(float*)(dcm[1]));
	float delta[2][3];
	vector3d_scale(-err/2,(float*)(dcm[1]),(float*)(delta[0]));
	vector3d_scale(-err/2,(float*)(dcm[0]),(float*)(delta[1]));
	vector3d_add((float*)(dcm[0]),(float*)(delta[0]),(float*)(dcm[0]));
	vector3d_add((float*)(dcm[1]),(float*)(delta[1]),(float*)(dcm[1]));

	//Z = X x Y  (DCMDraft2 Eqn. 20) , 
	vector3d_cross((float*)(dcm[0]),(float*)(dcm[1]),(float*)(dcm[2]));
	//re-nomralization
	vector3d_normalize((float*)(dcm[0]));
	vector3d_normalize((float*)(dcm[1]));
	vector3d_normalize((float*)(dcm[2]));
}


//rotate DCM matrix by a small rotation given by angular rotation vector w
//see http://gentlenav.googlecode.com/files/DCMDraft2.pdf
void dcm_rotate(float dcm[3][3], float w[3]){
	//float W[3][3];	
	//creates equivalent skew symetric matrix plus identity matrix
	//vector3d_skew_plus_identity((float*)w,(float*)W);
	//float dcmTmp[3][3];
	//matrix_multiply(3,3,3,(float*)W,(float*)dcm,(float*)dcmTmp);
	
	int i;
	float dR[3];
	//update matrix using formula R(t+1)= R(t) + dR(t) = R(t) + w x R(t)
	for(i=0;i<3;i++){
		vector3d_cross(w,dcm[i],dR);
		vector3d_add(dcm[i],dR,dcm[i]);
	}		

	//make matrix orthonormal again
	dcm_orthonormalize(dcm);
}

#endif