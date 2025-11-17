import { useState } from 'react';
import { User, Mail, Crown, Calendar } from 'lucide-react';
import { useAuth } from '../hooks/useAuth';
import { SubscriptionTier } from '../types';
import { toast } from 'sonner';

export default function ProfilePage() {
  const { user, updateUser } = useAuth();
  const [editing, setEditing] = useState(false);
  const [formData, setFormData] = useState({
    name: user?.name || '',
    email: user?.email || '',
  });

  const handleSubmit = async (e: React.FormEvent) => {
    e.preventDefault();
    try {
      // TODO: API call
      toast.success('Perfil actualizado');
      setEditing(false);
    } catch (error) {
      toast.error('Error al actualizar el perfil');
    }
  };

  const getSubscriptionInfo = () => {
    switch (user?.subscription) {
      case SubscriptionTier.FREE:
        return {
          name: 'Gratis',
          color: 'text-gray-600',
          bg: 'bg-gray-100',
          features: [
            'Hasta 3 miembros familiares',
            'Inventario básico',
            '5 recetas por semana',
            'Calculadora nutricional',
          ],
        };
      case SubscriptionTier.PREMIUM:
        return {
          name: 'Premium',
          color: 'text-primary-600',
          bg: 'bg-primary-100',
          features: [
            'Miembros ilimitados',
            'Inventario ilimitado',
            'Recetas ilimitadas con IA',
            'Escaneo de tickets',
            'Planificación automática',
            'Soporte prioritario',
          ],
        };
      case SubscriptionTier.PROFESSIONAL:
        return {
          name: 'Profesional',
          color: 'text-purple-600',
          bg: 'bg-purple-100',
          features: [
            'Todo lo de Premium',
            'Perfil en marketplace',
            'Gestión de pacientes',
            'Analytics y reportes',
            '15% comisión por consulta',
          ],
        };
      default:
        return {
          name: 'Gratis',
          color: 'text-gray-600',
          bg: 'bg-gray-100',
          features: [],
        };
    }
  };

  const subscriptionInfo = getSubscriptionInfo();

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold text-gray-900">Mi Perfil</h1>
        <p className="text-gray-600 mt-1">Gestiona tu información personal y suscripción</p>
      </div>

      {/* Profile Card */}
      <div className="card">
        <div className="flex items-start justify-between mb-6">
          <div className="flex items-center space-x-4">
            <div className="h-20 w-20 rounded-full bg-primary-100 flex items-center justify-center">
              <User className="h-10 w-10 text-primary-600" />
            </div>
            <div>
              <h2 className="text-2xl font-bold">{user?.name}</h2>
              <p className="text-gray-600">{user?.email}</p>
              <div className="flex items-center space-x-2 mt-2">
                <span className={`px-3 py-1 rounded-full text-sm font-medium ${subscriptionInfo.bg} ${subscriptionInfo.color}`}>
                  {subscriptionInfo.name}
                </span>
              </div>
            </div>
          </div>
          <button
            onClick={() => setEditing(!editing)}
            className="btn-outline text-sm"
          >
            {editing ? 'Cancelar' : 'Editar'}
          </button>
        </div>

        {editing ? (
          <form onSubmit={handleSubmit} className="space-y-4">
            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">
                Nombre completo
              </label>
              <input
                type="text"
                value={formData.name}
                onChange={(e) => setFormData({ ...formData, name: e.target.value })}
                className="input"
                required
              />
            </div>

            <div>
              <label className="block text-sm font-medium text-gray-700 mb-1">
                Email
              </label>
              <input
                type="email"
                value={formData.email}
                onChange={(e) => setFormData({ ...formData, email: e.target.value })}
                className="input"
                required
              />
            </div>

            <div className="flex space-x-3 pt-4">
              <button
                type="button"
                onClick={() => setEditing(false)}
                className="btn-secondary flex-1"
              >
                Cancelar
              </button>
              <button type="submit" className="btn-primary flex-1">
                Guardar Cambios
              </button>
            </div>
          </form>
        ) : (
          <div className="grid md:grid-cols-2 gap-4 pt-4 border-t">
            <div>
              <div className="text-sm text-gray-600 mb-1">Nombre</div>
              <div className="font-medium">{user?.name}</div>
            </div>
            <div>
              <div className="text-sm text-gray-600 mb-1">Email</div>
              <div className="font-medium">{user?.email}</div>
            </div>
            <div>
              <div className="text-sm text-gray-600 mb-1">Rol</div>
              <div className="font-medium capitalize">{user?.role}</div>
            </div>
            <div>
              <div className="text-sm text-gray-600 mb-1">Miembro desde</div>
              <div className="font-medium">
                {user?.createdAt ? new Date(user.createdAt).toLocaleDateString('es') : 'N/A'}
              </div>
            </div>
          </div>
        )}
      </div>

      {/* Subscription Card */}
      <div className="card">
        <div className="flex items-center space-x-2 mb-4">
          <Crown className="h-5 w-5 text-primary-600" />
          <h3 className="text-xl font-semibold">Suscripción</h3>
        </div>

        <div className="mb-6">
          <div className="flex items-center justify-between mb-2">
            <span className="text-2xl font-bold">{subscriptionInfo.name}</span>
            {user?.subscription !== SubscriptionTier.FREE && (
              <span className="text-gray-600">$4.99/mes</span>
            )}
          </div>
        </div>

        <div className="space-y-2 mb-6">
          <div className="text-sm font-medium text-gray-700 mb-3">Características incluidas:</div>
          {subscriptionInfo.features.map((feature, index) => (
            <div key={index} className="flex items-start">
              <svg
                className="h-5 w-5 text-primary-600 mr-2 flex-shrink-0 mt-0.5"
                fill="none"
                strokeLinecap="round"
                strokeLinejoin="round"
                strokeWidth="2"
                viewBox="0 0 24 24"
                stroke="currentColor"
              >
                <path d="M5 13l4 4L19 7"></path>
              </svg>
              <span className="text-gray-700">{feature}</span>
            </div>
          ))}
        </div>

        {user?.subscription === SubscriptionTier.FREE ? (
          <button className="btn-primary w-full">
            Actualizar a Premium
          </button>
        ) : (
          <button className="btn-outline w-full">
            Gestionar Suscripción
          </button>
        )}
      </div>

      {/* Stats Card */}
      <div className="card">
        <h3 className="text-xl font-semibold mb-4">Estadísticas</h3>
        <div className="grid grid-cols-2 md:grid-cols-4 gap-4">
          <div>
            <div className="text-3xl font-bold text-primary-600">24</div>
            <div className="text-sm text-gray-600">Recetas guardadas</div>
          </div>
          <div>
            <div className="text-3xl font-bold text-primary-600">45</div>
            <div className="text-sm text-gray-600">Items en inventario</div>
          </div>
          <div>
            <div className="text-3xl font-bold text-primary-600">3</div>
            <div className="text-sm text-gray-600">Miembros familiares</div>
          </div>
          <div>
            <div className="text-3xl font-bold text-primary-600">7</div>
            <div className="text-sm text-gray-600">Comidas planeadas</div>
          </div>
        </div>
      </div>
    </div>
  );
}
