import { Link } from 'react-router-dom';
import { ChefHat, Camera, Calendar, Users, Sparkles, ShoppingCart } from 'lucide-react';

export default function HomePage() {
  return (
    <div className="min-h-screen bg-white">
      {/* Hero Section */}
      <header className="bg-gradient-to-br from-primary-600 to-primary-700 text-white">
        <nav className="container mx-auto px-4 py-6 flex items-center justify-between">
          <div className="flex items-center space-x-2">
            <ChefHat className="h-8 w-8" />
            <span className="text-xl font-bold">Y Qué Comemos</span>
          </div>
          <div className="space-x-4">
            <Link to="/login" className="btn btn-secondary text-gray-900">
              Iniciar Sesión
            </Link>
            <Link to="/register" className="btn bg-white text-primary-600 hover:bg-gray-100">
              Registrarse
            </Link>
          </div>
        </nav>

        <div className="container mx-auto px-4 py-20 text-center">
          <h1 className="text-5xl md:text-6xl font-bold mb-6">
            ¿Qué comemos hoy?
          </h1>
          <p className="text-xl md:text-2xl mb-8 text-primary-100 max-w-3xl mx-auto">
            Planifica las comidas de tu familia, gestiona tu inventario y descubre
            recetas personalizadas con inteligencia artificial
          </p>
          <Link
            to="/register"
            className="btn btn-primary bg-white text-primary-600 hover:bg-gray-100 text-lg px-8 py-3"
          >
            Comenzar Gratis
          </Link>
        </div>
      </header>

      {/* Features Section */}
      <section className="container mx-auto px-4 py-20">
        <h2 className="text-3xl font-bold text-center mb-12">
          Todo lo que necesitas para cocinar mejor
        </h2>

        <div className="grid md:grid-cols-2 lg:grid-cols-3 gap-8">
          <FeatureCard
            icon={<Camera className="h-8 w-8" />}
            title="Inventario Inteligente"
            description="Escanea tickets, toma fotos o ingresa manualmente los alimentos que tienes en casa"
          />
          <FeatureCard
            icon={<Sparkles className="h-8 w-8" />}
            title="Recetas con IA"
            description="Genera recetas personalizadas basadas en lo que tienes disponible y las preferencias de tu familia"
          />
          <FeatureCard
            icon={<Calendar className="h-8 w-8" />}
            title="Planificación Automática"
            description="Organiza todas tus comidas semanales optimizadas para cada miembro de la familia"
          />
          <FeatureCard
            icon={<Users className="h-8 w-8" />}
            title="Gestión Familiar"
            description="Perfiles personalizados con necesidades nutricionales específicas para cada miembro"
          />
          <FeatureCard
            icon={<ShoppingCart className="h-8 w-8" />}
            title="Lista de Compras"
            description="Recibe sugerencias inteligentes de qué comprar para completar tus recetas favoritas"
          />
          <FeatureCard
            icon={<ChefHat className="h-8 w-8" />}
            title="Nutricionistas Pro"
            description="Conecta con profesionales certificados para obtener asesoría personalizada"
          />
        </div>
      </section>

      {/* Pricing Section */}
      <section className="bg-gray-50 py-20">
        <div className="container mx-auto px-4">
          <h2 className="text-3xl font-bold text-center mb-12">
            Planes para todos
          </h2>

          <div className="grid md:grid-cols-3 gap-8 max-w-5xl mx-auto">
            <PricingCard
              name="Gratis"
              price="$0"
              features={[
                'Hasta 3 miembros familiares',
                'Inventario básico',
                '5 recetas por semana',
                'Calculadora nutricional'
              ]}
            />
            <PricingCard
              name="Premium"
              price="$4.99"
              period="/mes"
              featured
              features={[
                'Miembros ilimitados',
                'Inventario ilimitado',
                'Recetas ilimitadas con IA',
                'Escaneo de tickets',
                'Planificación automática',
                'Soporte prioritario'
              ]}
            />
            <PricingCard
              name="Profesional"
              price="$19.99"
              period="/mes"
              features={[
                'Todo lo de Premium',
                'Perfil en marketplace',
                'Gestión de pacientes',
                'Analytics y reportes',
                '15% comisión por consulta'
              ]}
            />
          </div>
        </div>
      </section>

      {/* Footer */}
      <footer className="bg-gray-900 text-white py-12">
        <div className="container mx-auto px-4 text-center">
          <div className="flex items-center justify-center space-x-2 mb-4">
            <ChefHat className="h-6 w-6" />
            <span className="text-lg font-bold">Y Qué Comemos</span>
          </div>
          <p className="text-gray-400">
            © 2024 Y Qué Comemos. Todos los derechos reservados.
          </p>
        </div>
      </footer>
    </div>
  );
}

function FeatureCard({ icon, title, description }: { icon: React.ReactNode; title: string; description: string }) {
  return (
    <div className="card hover:shadow-lg transition-shadow">
      <div className="text-primary-600 mb-4">{icon}</div>
      <h3 className="text-xl font-semibold mb-2">{title}</h3>
      <p className="text-gray-600">{description}</p>
    </div>
  );
}

function PricingCard({
  name,
  price,
  period = '',
  features,
  featured = false
}: {
  name: string;
  price: string;
  period?: string;
  features: string[];
  featured?: boolean;
}) {
  return (
    <div className={`card ${featured ? 'border-primary-600 border-2 shadow-lg' : ''}`}>
      {featured && (
        <div className="text-center mb-4">
          <span className="bg-primary-600 text-white px-3 py-1 rounded-full text-sm font-medium">
            Más Popular
          </span>
        </div>
      )}
      <div className="text-center mb-6">
        <h3 className="text-xl font-semibold mb-2">{name}</h3>
        <div className="text-4xl font-bold">
          {price}
          <span className="text-lg text-gray-600">{period}</span>
        </div>
      </div>
      <ul className="space-y-3 mb-6">
        {features.map((feature, index) => (
          <li key={index} className="flex items-start">
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
          </li>
        ))}
      </ul>
      <Link
        to="/register"
        className={`btn w-full ${featured ? 'btn-primary' : 'btn-outline'}`}
      >
        Comenzar
      </Link>
    </div>
  );
}
