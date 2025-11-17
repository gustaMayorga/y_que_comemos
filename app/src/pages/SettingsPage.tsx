import { useState } from 'react';
import { Bell, Lock, Globe, Trash2 } from 'lucide-react';
import { toast } from 'sonner';

export default function SettingsPage() {
  const [notifications, setNotifications] = useState({
    mealReminders: true,
    expirationAlerts: true,
    recipesuggestions: false,
    promotions: false,
  });

  const [language, setLanguage] = useState('es');

  const handleNotificationChange = (key: keyof typeof notifications) => {
    setNotifications({ ...notifications, [key]: !notifications[key] });
    toast.success('Configuración actualizada');
  };

  const handleDeleteAccount = () => {
    if (window.confirm('¿Estás seguro de que quieres eliminar tu cuenta? Esta acción no se puede deshacer.')) {
      // TODO: API call
      toast.error('Función no implementada aún');
    }
  };

  return (
    <div className="max-w-4xl mx-auto space-y-6">
      {/* Header */}
      <div>
        <h1 className="text-3xl font-bold text-gray-900">Configuración</h1>
        <p className="text-gray-600 mt-1">Personaliza tu experiencia en la aplicación</p>
      </div>

      {/* Notifications */}
      <div className="card">
        <div className="flex items-center space-x-2 mb-4">
          <Bell className="h-5 w-5 text-primary-600" />
          <h3 className="text-xl font-semibold">Notificaciones</h3>
        </div>

        <div className="space-y-4">
          <SettingToggle
            label="Recordatorios de comidas"
            description="Recibe notificaciones para tus comidas planeadas"
            checked={notifications.mealReminders}
            onChange={() => handleNotificationChange('mealReminders')}
          />
          <SettingToggle
            label="Alertas de vencimiento"
            description="Te avisamos cuando los alimentos están por vencer"
            checked={notifications.expirationAlerts}
            onChange={() => handleNotificationChange('expirationAlerts')}
          />
          <SettingToggle
            label="Sugerencias de recetas"
            description="Recibe recomendaciones basadas en tu inventario"
            checked={notifications.recipesuggestions}
            onChange={() => handleNotificationChange('recipesuggestions')}
          />
          <SettingToggle
            label="Promociones y novedades"
            description="Mantente informado de nuevas características"
            checked={notifications.promotions}
            onChange={() => handleNotificationChange('promotions')}
          />
        </div>
      </div>

      {/* Language */}
      <div className="card">
        <div className="flex items-center space-x-2 mb-4">
          <Globe className="h-5 w-5 text-primary-600" />
          <h3 className="text-xl font-semibold">Idioma</h3>
        </div>

        <select
          value={language}
          onChange={(e) => {
            setLanguage(e.target.value);
            toast.success('Idioma actualizado');
          }}
          className="input max-w-xs"
        >
          <option value="es">Español</option>
          <option value="en">English</option>
          <option value="pt">Português</option>
        </select>
      </div>

      {/* Security */}
      <div className="card">
        <div className="flex items-center space-x-2 mb-4">
          <Lock className="h-5 w-5 text-primary-600" />
          <h3 className="text-xl font-semibold">Seguridad</h3>
        </div>

        <div className="space-y-4">
          <div className="flex items-center justify-between pb-4 border-b">
            <div>
              <div className="font-medium">Cambiar contraseña</div>
              <div className="text-sm text-gray-600">Actualiza tu contraseña regularmente</div>
            </div>
            <button className="btn-outline text-sm">Cambiar</button>
          </div>

          <div className="flex items-center justify-between">
            <div>
              <div className="font-medium">Autenticación de dos factores</div>
              <div className="text-sm text-gray-600">Agrega una capa extra de seguridad</div>
            </div>
            <button className="btn-outline text-sm">Configurar</button>
          </div>
        </div>
      </div>

      {/* Danger Zone */}
      <div className="card border-red-200">
        <div className="flex items-center space-x-2 mb-4">
          <Trash2 className="h-5 w-5 text-red-600" />
          <h3 className="text-xl font-semibold text-red-600">Zona de Peligro</h3>
        </div>

        <div className="flex items-center justify-between">
          <div>
            <div className="font-medium">Eliminar cuenta</div>
            <div className="text-sm text-gray-600">
              Esta acción es permanente y no se puede deshacer
            </div>
          </div>
          <button
            onClick={handleDeleteAccount}
            className="btn bg-red-600 text-white hover:bg-red-700 text-sm"
          >
            Eliminar
          </button>
        </div>
      </div>
    </div>
  );
}

function SettingToggle({
  label,
  description,
  checked,
  onChange,
}: {
  label: string;
  description: string;
  checked: boolean;
  onChange: () => void;
}) {
  return (
    <div className="flex items-center justify-between py-3">
      <div className="flex-1">
        <div className="font-medium">{label}</div>
        <div className="text-sm text-gray-600">{description}</div>
      </div>
      <button
        onClick={onChange}
        className={`
          relative inline-flex h-6 w-11 flex-shrink-0 cursor-pointer rounded-full border-2 border-transparent
          transition-colors duration-200 ease-in-out focus:outline-none focus:ring-2 focus:ring-primary-600 focus:ring-offset-2
          ${checked ? 'bg-primary-600' : 'bg-gray-200'}
        `}
      >
        <span
          className={`
            pointer-events-none inline-block h-5 w-5 transform rounded-full bg-white shadow ring-0
            transition duration-200 ease-in-out
            ${checked ? 'translate-x-5' : 'translate-x-0'}
          `}
        />
      </button>
    </div>
  );
}
