
__kernel 
void calculateForces(__global float3 *pos, __global float3 *vel, __global float3 *acc ,__global float *mass, __global float3 *new_acc)
{
    // Get the work item's unique ID
    int idx = get_global_id(0);
	new_acc[idx] = (float3)(0.0f, 0.0f,0.0f);
	float dx, dy, dz, r, f;
	if (idx >= 256) return;
	for (int j = 0; j < 256; ++j)
	{
			if (j == idx) continue;
			dx = pos[j].x - pos[idx].x;
			dy = pos[j].y - pos[idx].y;
			dz = pos[j].z - pos[idx].z;
			r = sqrt(dx*dx + dy*dy + dz*dz);
			
			f = (6.67384e-11* mass[j]*mass[idx]) / (r*r);
			new_acc[idx] += (float3)(pos[j]-pos[idx])/r*f;

	}	
}

__kernel void integrate(__global float3 *pos, __global float3 *vel, __global float3 *acc , __global float3 *new_pos, __global float3 *new_vel)
{
	int idx = get_global_id(0);
	if (idx >= 256) return;
	new_pos[idx].x = pos[idx].x + (vel[idx].x * 0.1f);
	new_pos[idx].y = pos[idx].y + (vel[idx].y * 0.1f);
	new_pos[idx].z = pos[idx].z + (vel[idx].z * 0.1f);

	new_vel[idx].x = vel[idx].x + (acc[idx].x * 0.1f);
	new_vel[idx].y = vel[idx].y + (acc[idx].y * 0.1f);
	new_vel[idx].z = vel[idx].z + (acc[idx].z * 0.1f);
}