//Chương trình vẽ 1 hình lập phương đơn vị theo mô hình lập trình OpenGL hiện đại

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/


// remember to prototype
void generateGeometry( void );
void initGPUBuffers( void );
void shaderSetup( void );
void display( void );
void keyboard( unsigned char key, int x, int y );


typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;

point4 points[NumPoints]; /* Danh sách các đỉnh của các tam giác cần vẽ*/
color4 colors[NumPoints]; /* Danh sách các màu tương ứng cho các đỉnh trên*/
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/

point4 vertices[8]; /* Danh sách 8 đỉnh của hình lập phương*/
color4 vertex_colors[8]; /*Danh sách các màu tương ứng cho 8 đỉnh hình lập phương*/

GLuint program;

void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gán giá trị màu sắc cho các đỉnh của hình lập phương	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry( void )
{
	initCube();
	makeColorCube();
}


void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

GLuint model_view_loc;



// hàm biến đổi màu săc
void changeColor(float red, float green, float blue) {
	for (int i = 0; i < 36; i++) {
		colors[i] = color4(red, green, blue, 1.0);
	}
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
}
GLint model_scale=1;// tỉ lệ mô hình.
GLfloat car_scale = 0.05;
mat4 model_codrinate;// vị trí trong không gian(tọa độ,tỉ lệ)

// thiết lập camera

	//các tham số cho model view
GLfloat radius = 0;
GLuint projection_loc, camera_loc;
GLfloat xeye=-3, yeye=3, zeye=23;
void setupCamera() {
	point4 eye(xeye,yeye,zeye, 1);
	point4 at(xeye - 0.5 * sin(radius),yeye, zeye - 0.5 * cos(radius), 1.0);
	vec4 up(0.0, 1.0, 0.0, 1.0);

	mat4 mv = LookAt(eye, at, up);
	glUniformMatrix4fv(camera_loc, 1, GL_TRUE, mv);

	GLfloat l = -0.25, r = 0.25;
	GLfloat bottom = -0.25, top = 0.25;
	GLfloat zNear = 0.5, zFar = 16.0;
	mat4 p = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);
	
}
// thiết lập hình dáng.

// nền đất.- minh duc
void platform() {
	mat4 instance = Translate(0,-0.25,0)*Scale(30, 0.5, 30);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	changeColor(0, 0.4, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

// cửa hàng cấp 4.-Minh Đức

// nền nhà- minh duc
void ground() {
	mat4 instance = Translate(0,0.5, 0)*Scale(10,1, 16);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	changeColor(0.4, 0.3, 0);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// tường - minh duc
void wall(mat4 mt) {
	
	mat4 instance = mt * Scale(1, 6, 16);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	changeColor(0.5, 0.5, 1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// tường :10*6*1;
 void wall1(mat4 mt) {
mat4 instance = mt * Scale(10, 6, 1);
glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
changeColor(0.5, 0.5, 1);
glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
 // tường :7.5*6*1
void wall2(mat4 mt) {
	 mat4 instance = mt * Scale(5, 6, 1);
	 glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	 changeColor(0.5, 0.5, 1);
	 glDrawArrays(GL_TRIANGLES, 0, NumPoints);
 }
// tường :3.5*6*1 - tuong o tren cua chinh
void wall3(mat4 mt) {
	mat4 instance = mt * Scale(3.5, 0.9, 1);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	changeColor(0.5, 0.5, 1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// bậc thang:
void bacthang(mat4 mt) {

	changeColor(0.3, 0.5, 0.2);
	mat4 instance = mt * Scale(3, 0.75, 0.25);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	
	changeColor(0.3, 0.5, 0.2);
	instance = mt*Translate(0,-0.125,0.25) * Scale(3, 0.5, 0.25);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	changeColor(0.3, 0.5, 0.2);
	instance = mt * Translate(0,-0.25 , 0.5) * Scale(3, 0.25, 0.25);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
// mái nhà

//vẽ tam giác

void tamgiac(int x,int y,int z,point4 *mainha,int *count,point4 dinh[]) {
	mainha[*count] = dinh[x];
	*count=*count+1;
	mainha[*count] = dinh[y];
	*count = *count + 1;
	mainha[*count] = dinh[z];
	*count = *count + 1;

}
void vemainha() {
	point4 mainha[24];
	int count = 0;
	//các đỉnh
	point4 dinh[6];
	dinh[0] = { -1,0,-1 ,1.0};
	dinh[1] = { 1,0,-1,1.0 };
	dinh[2] = { -1,0,1,1.0 };
	dinh[3] = { 1,0,1,1.0 };
	dinh[4] = { 0,0.8,1,1.0 };
	dinh[5] = { 0,0.8,-1,1.0 };
	tamgiac(0, 1, 2, &mainha[0], &count, dinh);
	tamgiac(1, 2, 4, &mainha[0], &count, dinh);
	tamgiac(2, 3, 4, &mainha[0], &count, dinh);
	tamgiac(0, 1, 5, &mainha[0], &count, dinh);
	tamgiac(0, 2, 5, &mainha[0], &count, dinh);
	tamgiac(2, 3, 5, &mainha[0], &count, dinh);
	tamgiac(1, 3, 4, &mainha[0], &count, dinh);
	tamgiac(1, 3, 5, &mainha[0], &count, dinh);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mainha), mainha);
	mat4 instance = Translate(0,7,0)*Scale(6,1.4,9);
	//mat4 instance = Translate(0, 7, 8) * Scale(6, 1.4, 9);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance);
	changeColor(0,0.4,0.4);
	vec4 color = { 0, 0, 0 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color); 
	glDrawArrays(GL_TRIANGLES, 0, 24);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
}
// tổng hợp - set cua hang - minh duc
void Myhome() {
	model_scale = 1;
	model_codrinate = Translate(0, 0, 0);
	mat4 mt = Translate(4.5, 4,0);
	wall(mt);
	mt = Translate(-4.5, 4, 0);
	wall(mt);
	mat4 mt1 = Translate(0,4,-7.5);
	wall1(mt1);
	mat4 mt2 = Translate(-5/2, 4, 7.5);
	wall2(mt2);
	mat4 mt3 = Translate(2.25, 6.55, 7.5);
	wall3(mt3);
	mat4 matrix_bacthang = Translate((1+2/2)+0.25,0.375,8.125);
	bacthang(matrix_bacthang);
	ground();
	vemainha();
}


//cửa chính.- minh duc
GLfloat theta_Cua[] = { 0,0 };
void Cua_Trai(mat4 instance , mat4 model_view, GLdouble Cua_Dai, GLdouble Cua_Rong, GLdouble Cua_Cao) {
	instance = Translate(0.5 * Cua_Dai, 0.5 * Cua_Cao, 0.0) * Scale(Cua_Dai, Cua_Cao, Cua_Rong);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Cua_Phai(mat4 instance, mat4 model_view, GLdouble Cua_Dai, GLdouble Cua_Rong, GLdouble Cua_Cao)
{
	instance = Translate(-0.5 * Cua_Dai, 0.5 * Cua_Cao, 0.0) * Scale(Cua_Dai, Cua_Cao, Cua_Rong);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Num_Trai(mat4 instance, mat4 model_view, GLdouble Num_Dai, GLdouble Num_Rong, GLdouble Num_Cao)
{
	instance = Translate(0.0, 0.5 * Num_Cao, 0.1)
		* Scale(Num_Dai, Num_Cao, Num_Rong);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void Num_Phai(mat4 instance, mat4 model_view, GLdouble Num_Dai, GLdouble Num_Rong, GLdouble Num_Cao)
{
	instance = Translate(0.0, 0.5 * Num_Cao, 0.0)
		* Scale(Num_Dai, Num_Cao, Num_Rong);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, model_view * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void BoCua(mat4 instance, mat4 model_view, GLdouble Num_Dai, GLdouble Num_Rong, GLdouble Num_Cao, GLdouble Cua_Dai, GLdouble Cua_Rong, GLdouble Cua_Cao) {
	model_view = model_codrinate*Translate(-Cua_Dai, 0.0, 0.0) * RotateY(theta_Cua[0]);
	changeColor(1, 0, 0);
	Cua_Trai(instance,model_view,Cua_Dai, Cua_Rong,Cua_Cao);
	model_view = model_view * Translate(Cua_Dai - Num_Dai, 0.4 * Cua_Cao, -0.5 * Cua_Rong);
	changeColor(1, 1, 0);
	Num_Trai(instance, model_view, Num_Dai, Num_Rong, Num_Cao);

	model_view = model_codrinate * Translate(Cua_Dai, 0.0, 0.0) * RotateY(theta_Cua[1]);
	changeColor(1, 0, 1);
	Cua_Phai(instance, model_view, Cua_Dai, Cua_Rong, Cua_Cao);
	model_view = model_view * Translate(-(Cua_Dai - Num_Dai), 0.4 * Cua_Cao, 0.5 * Cua_Rong);
	changeColor(0.6, 0.8, 0.4);
	Num_Phai(instance, model_view, Num_Dai, Num_Rong, Num_Cao);

}
void setBoCua() {
	vec4 color = { 0, 0, 0 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(1, 0, 0);
	model_codrinate = Translate(2.228, 1, 8);
	model_scale = 17;

	GLdouble Cua_Dai = 0.1015 * model_scale, Cua_Rong = 0.01 * model_scale, Cua_Cao = 0.3 * model_scale,
		Num_Dai = 0.01 * model_scale, Num_Rong = 0.02 * model_scale, Num_Cao = 0.01 * model_scale;
	mat4 instance;
	mat4 model_view;

	BoCua(instance, model_view, Num_Dai, Num_Rong, Num_Cao, Cua_Dai, Cua_Rong, Cua_Cao);
}

// tao xe oto cho hang - aka Minh Duc
GLfloat theta1 = 0, theta2 = 0, theta3 = 0, ctphai = 0, cttrai = 0, strai = 0, sphai = 0, banhxelan = 0;
GLfloat run_x = 0, locX = 0, locY = 0, locZ = 0;
GLfloat  locXP = 0, locYP = 0, locZP = 0;
GLfloat  locXTP = 0, locYTP = 0, locZTP = 0;
GLfloat  locXTT = 0, locYTT = 0, locZTT = 0;
// 2 mặt 2 bên thùng xe
void thanhchandoc1(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	changeColor(0, 0.5, 0.5);
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x, y, z) * Scale(1 * model_scale * car_scale, 0.4 * model_scale * car_scale, 0.05 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	
}
// mặt trên thùng xe
void matthungxe(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x, y, z) * Scale(1 * model_scale * car_scale, 0.05 * model_scale * car_scale, 0.55 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// mặt chắn trước của thùng xe
void matdoc(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x, y, z) * Scale(0.05 * model_scale * car_scale, 0.5 * model_scale * car_scale, 0.55 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// gầm xe
void gamxe(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x, y, z) * Scale(1.5 * model_scale * car_scale, 0.1 * model_scale * car_scale, 0.55 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// đầu xe
void dauxe(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x, y, z) * Scale(0.475 * model_scale*car_scale, 0.4 * model_scale * car_scale, 0.55 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// cửa sau xe bên trái
void cuasauxet(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	vec4 color = { 0, 1, 0 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(0, 0, 0);
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x + (locX*model_scale*car_scale), y + (locY * model_scale * car_scale), z + (locZ * model_scale * car_scale)) * RotateY(strai) * Scale(0.02 * model_scale * car_scale, 0.4 * model_scale * car_scale, 0.25 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// cửa sau xe bên phải
void cuasauxep(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	vec4 color = { 0.5, 0.5, 0 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(0, 0, 0);
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x + locXP, y + locYP, z + locZP) * RotateY(sphai) * Scale(0.02 * model_scale * car_scale, 0.4 * model_scale * car_scale, 0.25 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// cửa xe trước bên phải
void cuaxephai(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	vec4 color = { 1, 1, 1 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(0, 0, 0);
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0)* Translate(x + locXTP, y + locYTP, z + locZTP) * RotateY(ctphai) * Scale(0.35 * model_scale * car_scale, 0.35 * model_scale * car_scale, 0.01 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// cửa xe trước bên trái
void cuaxetrai(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	vec4 color = { 1, 1, 1 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(0, 0, 0);
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x + locXTT, y + locYTT, z + locZTT) * RotateY(cttrai) * Scale(0.35 * model_scale * car_scale, 0.35 * model_scale * car_scale, 0.01 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
// cửa kính của xe
void cuakinh(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	vec4 color = { 1, 1, 1 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(0, 0, 0);
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x, y, z) * Scale(0.01 * model_scale * car_scale, 0.25 * model_scale * car_scale, 0.5 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void cubedonvibanhxe(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z, GLfloat rota)
{
	instance = model_codrinate * Translate(run_x * model_scale * car_scale, 0, 0) * Translate(x, y, z) * RotateZ(rota) * RotateZ(banhxelan) * Scale(0.2 * model_scale * car_scale, 0.01 * model_scale * car_scale, 0.1 * model_scale * car_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void taobanhxe(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)
{
	vec4 color = { 0, 0, 1 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(0, 0, 0);
	GLfloat tem = 0;
	for (int i = 0; i < 100; i++)
	{
		tem += 3.6;
		cubedonvibanhxe(instance,model_view,x, y, z, tem);
		if (i % 10 == 0)// giup banh xe hien thi mau khac
		{
			vec4 color = { 1, 0, 1 };
			glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
			changeColor(0, 0, 0);
		}
	}
}
void taoxe(mat4 instance, mat4 model_view) {
	model_view = RotateY(0) * RotateX(0) * RotateZ(0);

	thanhchandoc1(instance,model_view,0.25*model_scale * car_scale, 0 * model_scale * car_scale, 0.25 * model_scale * car_scale);
	thanhchandoc1(instance, model_view, 0.25 * model_scale * car_scale, 0 * model_scale * car_scale, -0.25 * model_scale * car_scale);
	matthungxe(instance, model_view, 0.25 * model_scale * car_scale, 0.225 * model_scale * car_scale, 0 * model_scale * car_scale);
	matthungxe(instance, model_view, 0.25 * model_scale * car_scale, -0.225 * model_scale * car_scale, 0 * model_scale * car_scale);
	matdoc(instance, model_view, -0.25 * model_scale * car_scale, 0 * model_scale * car_scale, 0 * model_scale * car_scale);
	gamxe(instance, model_view, 0 * model_scale * car_scale, -0.3 * model_scale * car_scale, 0 * model_scale * car_scale);
	dauxe(instance, model_view, -0.51 * model_scale * car_scale, -0.05 * model_scale * car_scale, 0 * model_scale * car_scale);
	cuasauxet(instance, model_view, 0.735 * model_scale * car_scale, 0 * model_scale * car_scale, -0.1 * model_scale * car_scale);
	cuasauxep(instance, model_view, 0.735 * model_scale * car_scale, 0 * model_scale * car_scale, 0.1 * model_scale * car_scale);
	cuaxephai(instance, model_view, -0.5 * model_scale * car_scale, -0.07 * model_scale * car_scale, 0.28 * model_scale * car_scale);
	cuaxetrai(instance, model_view, -0.5 * model_scale * car_scale, -0.07 * model_scale * car_scale, -0.28 * model_scale * car_scale);
	cuakinh(instance, model_view, -0.75 * model_scale * car_scale, -0.05 * model_scale * car_scale, 0 * model_scale * car_scale);
	taobanhxe(instance, model_view, -0.5 * model_scale * car_scale, -0.35 * model_scale * car_scale, -0.25 * model_scale * car_scale);//banh xe 1
	taobanhxe(instance, model_view, -0.5 * model_scale * car_scale, -0.35 * model_scale * car_scale, 0.25 * model_scale * car_scale);//banh xe 2
	taobanhxe(instance, model_view, 0.5 * model_scale * car_scale, -0.35 * model_scale * car_scale, 0.25 * model_scale * car_scale);//banh xe 3
	taobanhxe(instance, model_view, 0.5 * model_scale * car_scale, -0.35 * model_scale * car_scale, -0.25*model_scale * car_scale);//banh xe 4
}
void setxeDuc()
{
	model_codrinate = Translate(2, 2.41, 1);// vi tri cua xe trong khong gian
	mat4 instance; mat4 model_view;
	taoxe(instance, model_view);
}

// end tao xe
// 
// tao ra mat troi - nguon sang
void sun() {
	vec4 color = { 0.8,0.9,0.7 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	GLfloat rad = 0.4049 + radius;
	mat4 ctm = Translate(-30.463 * sin(rad) + xeye, 10.5 + yeye, -30.463 * cos(rad) + zeye);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, ctm);
	glutSolidSphere(1, 64, 64);
}

// tao ra gia de xe
void khungdoc(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z)// cot doc cua gia do
{
	instance = model_codrinate * Translate(x, y, z) * Scale(0.01 * model_scale , 0.5 * model_scale , 0.01 * model_scale );
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void matphangdexe(mat4 instance, mat4 model_view, GLfloat x, GLfloat y, GLfloat z, GLfloat dai)// mat phang de xe
{
	instance = model_codrinate * Translate(x, y, z) * Scale(0.48 * model_scale*dai , 0.001 * model_scale , 0.1 * model_scale);
	glUniformMatrix4fv(model_view_loc, 1, GL_TRUE, instance * model_view);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void taogia(mat4 instance, mat4 model_view) // tao gia thu 1
{
	model_view = RotateY(0) * RotateX(0) * RotateZ(0);
	khungdoc(instance, model_view, -0.2 * model_scale , 0 * model_scale , -0.25 * model_scale);//khung doc 1
	khungdoc(instance, model_view, -0.2 * model_scale, 0 * model_scale, -0.35 * model_scale);//khung doc 2
	khungdoc(instance, model_view, 0.26 * model_scale, 0 * model_scale, -0.25 * model_scale);//khung doc 3
	khungdoc(instance, model_view, 0.26 * model_scale, 0 * model_scale, -0.35 * model_scale);//khung doc 4
	matphangdexe(instance, model_view, 0.03 * model_scale, 0 * model_scale, -0.30 * model_scale,1.0);// mat phang 1
	matphangdexe(instance, model_view, 0.03 * model_scale, 0.1 * model_scale, -0.30 * model_scale,1.0);// mat phang 2
}
void taogia2(mat4 instance, mat4 model_view)// tao gia thu 2
{
	model_view = RotateY(0) * RotateX(0) * RotateZ(0);
	khungdoc(instance, model_view, 0 * model_scale, 0 * model_scale, -0.25 * model_scale);//khung doc 1
	khungdoc(instance, model_view, 0 * model_scale, 0 * model_scale, -0.35 * model_scale);//khung doc 2
	khungdoc(instance, model_view, 0.26 * model_scale, 0 * model_scale, -0.25 * model_scale);//khung doc 3
	khungdoc(instance, model_view, 0.26 * model_scale, 0 * model_scale, -0.35 * model_scale);//khung doc 4
	matphangdexe(instance, model_view, 0.13 * model_scale, 0 * model_scale, -0.30 * model_scale,0.6);// mat phang 1
	matphangdexe(instance, model_view, 0.13 * model_scale, 0.1 * model_scale, -0.30 * model_scale,0.6);//mat phang 2
}
void setgia()// them gia 1 vao khong gian
{
	vec4 color = { 0, 0.5, 1 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(0, 0, 0);
	model_codrinate = Translate(-0.5, 2, -1);// vi tri cua gia trong khong gian
	mat4 instance; mat4 model_view;
	taogia(instance, model_view);
}
void setgia2()// them gia 2 vao khong gian
{
	vec4 color = { 0, 0.5, 1 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	changeColor(0, 0, 0);
	model_codrinate = Translate(-0.5, 2, 6);// vi tri cua gia trong khong gian
	mat4 instance; mat4 model_view;
	taogia2(instance, model_view);
}

void display( void )
{
	
    glClear( GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT ); 
	vec4 color = { 0.0,0.0,0.0 };
	glUniform4fv(glGetUniformLocation(program, "colorValues"), 1, color);
	setupCamera();
      // vẽ các hình
	platform();// nền đất
	Myhome();
	setBoCua();
	//sun();
	setxeDuc();
	setgia();
	setgia2();
	glutSwapBuffers();
}


void keyboard( unsigned char key, int x, int y )
{
	// keyboard handler

    switch ( key ) {
		//di chuyển
	case 'd':
		zeye -= cos(3.1416*3/2+radius) * 0.5;
		xeye -= sin(3.1416*3 / 2+radius) * 0.5;
		break;
	case 'w':
		zeye -= cos(radius) * 0.5;
		xeye -= sin(radius) * 0.5;
		break;
	case 'a':
		zeye -= cos(radius+ 3.1416 / 2 ) * 0.5;
		xeye -= sin(radius+ 3.1416 / 2 ) * 0.5;
		break;
	case 's':
		zeye -= cos(radius + 3.1416) * 0.5;
		xeye -= sin(radius + 3.1416) * 0.5;
		break;
		// end di chuyển
	case 'q':// quay mặt sang trái
		radius += 0.1;
		break;
	case 'e'://quay mặt sang phải
		radius -= 0.1;
		break;
	
	case 'o':// dong cua trai
		if (theta_Cua[0] <= 90) {
			theta_Cua[0] += 5;
		}
		break;
	case 'O':// mo cua trai
		if (theta_Cua[0] >= -90) {
			theta_Cua[0] -= 5;
		}
		break;


	case 'P':// dong cua phai
		if (theta_Cua[1] <= 90) {
			theta_Cua[1] += 5;
		}

		break;
	case 'p':// mo cua chinh phai
		if (theta_Cua[1] >= -90) {
			theta_Cua[1] -= 5;
		}
		break;
		// dieu khien xe /
		/////////////////
	case '8':	// di chuyen xe ve truoc
		banhxelan += 10;// xoay banh xe nguoc chieu kim dong ho
		run_x -= 0.01;
		glutPostRedisplay();		// quit program
		break;
	case '5':	// di chuyen xe ve sau
		banhxelan -= 10;// xoay banh xe cung chieu kim dong ho
		run_x += 0.01;
		glutPostRedisplay();
		break;
	case 'v':	// mở cua sau trai	
		strai += 30;
		if (strai > 90)
		{
			strai -= 30;
			break;
		}
		else
		{
			locX += (0.12 / 3);
			//locYP += (0.4 / 3);
			locZ -= (0.15 / 3);
		}
		glutPostRedisplay();		
		break;
	case 'V':	// dong cua sau trai	
		strai -= 30;
		if (strai < 0)
		{
			strai += 30;
			break;
		}
		else
		{
			locX -= (0.12 / 3);
			//locYP += (0.4 / 3);
			locZ += (0.15 / 3);
		}
		glutPostRedisplay();		// quit program
		break;
	case 'b'://mo cua sau phai
		//0.02, 0.4, 0.25
		sphai -= 30;
		if (sphai < -90)
		{
			sphai += 30;
			break;
		}
		else
		{
			locXP += (0.12 / 3);
			//locYP += (0.4 / 3);
			locZP += (0.15 / 3);
		}
		glutPostRedisplay();		
		break;
	case 'B'://dong cua sau phai
	//0.02, 0.4, 0.25
		sphai += 30;
		if (sphai > 0)
		{
			sphai -= 30;
			break;
		}
		else
		{
			locXP -= (0.12 / 3);
			//locYP += (0.4 / 3);
			locZP -= (0.15 / 3);
		}
		glutPostRedisplay();		
		break;
	case 'y'://mo cua trước phải
			 //0.02, 0.4, 0.25
		ctphai -= 30;
		if (ctphai <= -90)
		{
			ctphai += 30;
			break;
		}
		else
		{
			locXTP -= (0.15 / 3);
			//locYP += (0.4 / 3);
			locZTP += (0.18 / 3);
		}
		glutPostRedisplay();		// quit program
		break;
	case 'Y':// dong cua trước phải
		 //0.02, 0.4, 0.25
		ctphai += 30;
		if (ctphai > 0)
		{
			ctphai -= 30;
			break;
		}
		else
		{
			locXTP += (0.15 / 3);
			//locYP += (0.4 / 3);
			locZTP -= (0.18 / 3);
		}
		glutPostRedisplay();		// quit program
		break;
	case 't':// mở cua trước trái
			 //0.02, 0.4, 0.25
		cttrai += 30;
		if (cttrai >= 90)
		{
			cttrai -= 30;
			break;
		}
		else
		{
			locXTT -= (0.15 / 3);
			//locYP += (0.4 / 3);
			locZTT -= (0.18 / 3);
		}
		glutPostRedisplay();		// quit program
		break;
	case 'T':// dong cua trước trái
		 //0.02, 0.4, 0.25
		cttrai -= 30;
		if (cttrai < 0)
		{
			cttrai += 30;
			break;
		}
		else
		{
			locXTT += (0.15 / 3);
			//locYP += (0.4 / 3);
			locZTT += (0.18 / 3);
		}
		glutPostRedisplay();		// quit program
		break;
	case 'm':
		glutIdleFunc(NULL);
		break;
	
    }

	glutPostRedisplay();
}


void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));

	/* Khởi tạo các tham số chiếu sáng - tô bóng*/
	point4 light_position(-10.0, 8.0, -15.0, 0.0);
	color4 light_ambient(0.2, 0.2, 0.2, 1.0);
	color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
	color4 light_specular(1.0, 1.0, 1.0, 1.0);;

	color4 material_ambient(1.0, 0.0, 1.0, 1.0);
	color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
	color4 material_specular(1.0, 0.8, 0.0, 1.0);
	float material_shininess = 100.0;

	color4 ambient_product = light_ambient * material_ambient;
	color4 diffuse_product = light_diffuse * material_diffuse;
	color4 specular_product = light_specular * material_specular;

	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);

	model_view_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	camera_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}

int main( int argc, char **argv )
{
	// main function: program starts here
	
    glutInit( &argc, argv );                       
    glutInitDisplayMode( GLUT_DOUBLE|GLUT_RGBA);
    glutInitWindowSize( 640, 640 );                 
	glutInitWindowPosition(100,150);               
    glutCreateWindow( "Drawing a car toy shop" );            

	glewInit();										

    generateGeometry( );                          
    initGPUBuffers( );							   
    shaderSetup( );                               
    glutDisplayFunc( display );                   
    glutKeyboardFunc( keyboard );                  

	glutMainLoop();
	
    return 0;
}
