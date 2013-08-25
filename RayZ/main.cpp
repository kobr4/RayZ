#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef struct {
	float position[3];
	float screen_position[3];
	float top_screen[3];
	float bottom_screen[3];
} t_camera;

typedef struct {
	float position[3];
	float color[3];
} t_light;

typedef struct {
	float position[3];
	float radius;
} t_sphere;

t_camera * createCamera() {
	t_camera * cam = (t_camera*)malloc(sizeof(t_camera));

	cam->position[0] = 0.f;
	cam->position[1] = 0.f;
	cam->position[2] = 0.f;

	cam->screen_position[0] = 0.f;
	cam->screen_position[1] = 0.f;
	cam->screen_position[2] = 1.f;

	cam->top_screen[0] = 1.0f;
	cam->top_screen[1] = 1.0f;
	cam->top_screen[2] = 0.0f;

	cam->bottom_screen[0] = -1.0f;
	cam->bottom_screen[1] = -1.0f;
	cam->bottom_screen[2] = 0.0f;

	return cam;
}

void vect_add(float * a, float * b) {
	a[0] = a[0] + b[0];
	a[1] = a[1] + b[1];
	a[2] = a[2] + b[2];
}

void vect_sub(float * a, float * b) {
	a[0] = a[0] - b[0];
	a[1] = a[1] - b[1];
	a[2] = a[2] - b[2];
}

void vect_mul(float * a, float * b) {
	a[0] = a[0] * b[0];
	a[1] = a[1] * b[1];
	a[2] = a[2] * b[2];
}


void normalize(float * a) {
	float length = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
	a[0] = a[0] / length;
	a[1] = a[1] / length;
	a[2] = a[2] / length;
}

float vect_dot(float * a, float * b) {
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

float collide_sphere(float * origin, float * direction, t_sphere * sphere) {
	float r2 = sphere->radius * sphere->radius;
	/*
	float oc[3];
	float a[3];
	oc[0] = origin[0];
	oc[1] = origin[1];
	oc[2] = origin[2];
	vect_sub(oc,sphere->position);
	a[0] = direction[0];
	a[1] = direction[1];
	a[2] = direction[2];
	vect_mul(a,oc);
	vect_mul(a,a);

	oc[0] = origin[0];
	oc[1] = origin[1];
	oc[2] = origin[2];
	vect_sub(oc,sphere->position);
	vect_mul(oc,oc);
	return a[0] + a[1] + a[2] - oc[0] - oc[1] - oc[2] + r2; 
	*/

	
	float a = direction[0] * direction[0] + direction[1] * direction[1] + direction[2] * direction[2];
	float oc[3];
	oc[0] = origin[0];
	oc[1] = origin[1];
	oc[2] = origin[2];
	vect_sub(oc,sphere->position);
	//vect_mul(oc,direction);
	float b = oc[0] * direction[0] + oc[1] * direction[1] + oc[2] * direction[2];
	b = b *2;
	oc[0] = origin[0];
	oc[1] = origin[1];
	oc[2] = origin[2];
	vect_sub(oc,sphere->position);
	float c = oc[0] * oc[0] + oc[1] * oc[1] + oc[2] * oc[2] - r2;

	return b*b - 4 * a * c;
	
}

unsigned int castRay(float * origin, float * direction, t_sphere * sphere_list,int spherecount, t_light * light_list, int lightcount) {
	float oc[3];
	float sphere_point[3];
	float light_direction[3];
	float sphere_normal[3];
	float color[3];
	float f;
	float d;
	for (int i = 0;i < spherecount;i++) {
		float res = collide_sphere(origin, direction, &sphere_list[i]);
		if (res >= 0.f) {
			// for each ray that touches a sphere, look for light
			// then check light collision;

			oc[0] = origin[0];
			oc[1] = origin[1];
			oc[2] = origin[2];
			vect_sub(oc,sphere_list[i].position);

			float bq = oc[0] * direction[0] + oc[1] * direction[1] + oc[2] * direction[2];
			bq = bq *2;

			//d = - oc[0] * direction[0] - oc[1] * direction[1] - oc[2] * direction[2] - sqrt(res);
			if (bq > 0.f)
				d = (- bq + sqrt(res)) * 0.5f;
			else 
				d = (- bq - sqrt(res)) * 0.5f;
			color[0] = 0.f;
			color[1] = 0.f;
			color[2] = 0.f;

			sphere_point[0] = origin[0] + direction[0] * d;
			sphere_point[1] = origin[1] + direction[1] * d;
			sphere_point[2] = origin[2] + direction[2] * d;
			
			sphere_normal[0] = sphere_point[0] - sphere_list[i].position[0];
			sphere_normal[1] = sphere_point[1] - sphere_list[i].position[1];
			sphere_normal[2] = sphere_point[2] - sphere_list[i].position[2];
			
			normalize(sphere_normal);
			
			for (int j = 0;j < lightcount;j++) {
				light_direction[0] = light_list[j].position[0] - sphere_point[0];
				light_direction[1] = light_list[j].position[1] - sphere_point[1];
				light_direction[2] = light_list[j].position[2] - sphere_point[2];
				normalize(light_direction);
				
				float lightocclusion = collide_sphere(sphere_point,light_direction,sphere_list);
				if (lightocclusion < 0.f) {
					color[0] += light_list[j].color[0] / (float)lightcount;
					color[1] += light_list[j].color[1] / (float)lightcount;
					color[2] += light_list[j].color[2] / (float)lightcount;
					color[0] = 0.f;
					color[1] = 0.f;
					color[2] = 0.f;
					return 0x3F3F3F;
				}
				
				f = vect_dot(sphere_normal, light_direction);
				if (f > 0.f) {
					/*
					color[0] += light_list[j].color[0] * f / (float)lightcount;
					color[1] += light_list[j].color[1] * f/ (float)lightcount;
					color[2] += light_list[j].color[2] * f/ (float)lightcount;
					*/

					color[0] += light_list[j].color[0] * f;
					color[1] += light_list[j].color[1] * f;
					color[2] += light_list[j].color[2] * f;
				} 
				
			}
			
			int r = (int)(color[0] * 255.0f);
			int g = (int)(color[1] * 255.0f);
			int b = (int)(color[2] * 255.0f);
			int bcolor = (r << 16) + (g << 8) + b ;
			if (bcolor < 0x3F3F3F)
				return 0x3F3F3F;
			//return 0xffffff;
			return bcolor;
		} 
	}
	
	//BGR
	return 0x000000;
}

void renderScene(unsigned char * map, int width, int height,t_camera * cam, t_sphere * sphere_list, int spherecount, t_light * light_list, int lightcount) {
	float v_size = cam->top_screen[0] - cam->bottom_screen[0];
	float v_step = v_size / (float)height;
	float h_size = cam->top_screen[1] - cam->bottom_screen[1];
	float h_step = h_size / (float)width;
	float direction[3];

	for (int j = 0;j < height;j++) {
			for (int i = 0;i < width;i++) {

			direction[0] = - h_size / 2.f + h_step * (float)i - cam->position[0];
			direction[1] = - v_size / 2.f + v_step * (float)j - cam->position[1];
			direction[2] = 1.f  - cam->position[2];
			normalize(direction);
			unsigned int color = castRay(cam->position,direction,sphere_list,spherecount, light_list, lightcount);			
			
			map[(i + j * width) * 3  + 0] = color & 0xff;
			map[(i + j * width) * 3  + 1] = (color & 0xff00) >> 8;
			map[(i + j * width) * 3  + 2] = (color & 0xff0000) >> 16;						
		}
	}
}

typedef struct {
	unsigned char id_length;
	unsigned char color_map_type;
	unsigned char image_type;
	unsigned char color_map_spec[5];
	unsigned char image_spec[10];
} t_tgaheader;

void writeTGAoutput(unsigned char * map, int width, int height) {
	FILE * pFile;
	pFile = fopen ("rayz.tga" , "wb");
	t_tgaheader header;

	header.id_length = 0;
	header.color_map_type = 0;
	header.image_type = 2;
	header.color_map_spec[0] = 0;
	header.color_map_spec[1] = 0;
	header.color_map_spec[2] = 0;
	header.color_map_spec[3] = 0;
	header.color_map_spec[4] = 0;	
	header.image_spec[0] = 0;
	header.image_spec[1] = 0;
	header.image_spec[2] = 0;
	header.image_spec[3] = 0;
	header.image_spec[4] = width & 0xff;
	header.image_spec[5] = (width & 0xff00) >> 8;
	header.image_spec[6] = height & 0xff;
	header.image_spec[7] = (height & 0xff00) >> 8;
	header.image_spec[8] = 24;
	header.image_spec[9] = 0;

	fwrite(&header,sizeof(t_tgaheader),1,pFile);
	fwrite(map,sizeof(unsigned char)*width*height*3,1,pFile);
	fclose(pFile);
}


int main (int argc, char** argv) {
	int width = 2048;
	int height = 2048;
	unsigned char * map = (unsigned char*) malloc(sizeof(unsigned char) * width * height * 3);


	t_camera * cam = createCamera(); 
	t_sphere * sphere_list = (t_sphere *)malloc(sizeof(t_sphere) * 2);
	t_light * light_list = (t_light*)malloc(sizeof(t_light) * 1);

	sphere_list[0].position[0] = 0.f;
	sphere_list[0].position[1] = 0.f;
	sphere_list[0].position[2] = 30.f;
	sphere_list[0].radius = 10.0f;
	
	sphere_list[1].position[0] = 0.0f;
	sphere_list[1].position[1] = -20.0f;
	sphere_list[1].position[2] = 30.f;
	sphere_list[1].radius = 5.0f;
	
	light_list[0].position[0] = 0.f;
	light_list[0].position[1] = 20.f;
	light_list[0].position[2] = 0.f;
	light_list[0].color[0] = 1.0f;
	light_list[0].color[1] = 1.0f;
	light_list[0].color[2] = 1.0f;

	renderScene(map,width,height,cam,sphere_list, 2, light_list, 1);

	writeTGAoutput(map, width, height);
	return 0;
}