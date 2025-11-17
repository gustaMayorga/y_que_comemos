import api from './api';
import { User } from '../types';

interface AuthResponse {
  user: User;
  token: string;
}

export async function login(email: string, password: string): Promise<AuthResponse> {
  const response = await api.post('/auth/login', { email, password });
  return response.data.data;
}

export async function register(email: string, password: string, name: string): Promise<AuthResponse> {
  const response = await api.post('/auth/register', { email, password, name });
  return response.data.data;
}

export async function validateToken(): Promise<User> {
  const response = await api.get('/auth/me');
  return response.data.data;
}

export async function updateProfile(data: Partial<User>): Promise<User> {
  const response = await api.put('/auth/profile', data);
  return response.data.data;
}
