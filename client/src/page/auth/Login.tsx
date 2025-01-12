import { Input } from "@/components/ui/input";
import { Button } from "@/components/ui/button";
import { z } from "zod";
import { useForm } from "react-hook-form";
import { zodResolver } from "@hookform/resolvers/zod";
import { useNavigate } from "react-router-dom";
import {
  Form,
  FormField,
  FormItem,
  FormLabel,
  FormControl,
  FormMessage,
} from "@/components/ui/form";
import { useAuth } from "@/hooks/useAuth";
import React from "react";
import LoadingSpinner from "@/components/common/layout/Spinner";

const loginSchema = z.object({
  email: z.string().email("Invalid email format"),
  password: z.string().min(3, "Password must be at least 3 characters"),
});

type LoginSchema = z.infer<typeof loginSchema>;

const Login = () => {
  const { checkAuth, login, user } = useAuth();

  const navigate = useNavigate();

  React.useEffect(() => {
    checkAuth();
  }, []);

  if (user) {
    navigate("/");
  }
  const form = useForm<LoginSchema>({
    resolver: zodResolver(loginSchema),
    defaultValues: {
      email: "",
      password: "",
    },
  });

  async function onSubmit(values: LoginSchema) {
    await login(values.email, values.password);
    navigate("/");
  }

  return (
    <div className="flex items-center justify-center h-screen">
      <Form {...form}>
        <form
          onSubmit={form.handleSubmit(onSubmit)}
          className="space-y-6 w-full max-w-md shadow-lg p-8 bg-white rounded-lg"
        >
          <h2 className="text-2xl font-semibold text-center text-gray-800 mb-4">
            Sign In
          </h2>

          {/* Email Field */}
          <FormField
            control={form.control}
            name="email"
            render={({ field }) => (
              <FormItem>
                <FormLabel className="text-gray-700">Email</FormLabel>
                <FormControl>
                  <Input
                    type="email"
                    placeholder="Enter your email"
                    className="border-2 border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 p-3 w-full"
                    {...field}
                  />
                </FormControl>
                <FormMessage className="text-red-500 text-sm" />
              </FormItem>
            )}
          />

          {/* Password Field */}
          <FormField
            control={form.control}
            name="password"
            render={({ field }) => (
              <FormItem className="relative">
                <FormLabel className="text-gray-700">Password</FormLabel>
                <FormControl>
                  <Input
                    type="password"
                    placeholder="Enter your password"
                    className="border-2 border-gray-300 rounded-lg focus:outline-none focus:ring-2 focus:ring-blue-500 p-3 w-full"
                    {...field}
                  />
                </FormControl>
                <FormMessage className="text-red-500 text-sm" />
              </FormItem>
            )}
          />

          {/* Submit Button */}
          <Button type="submit" className="w-full  text-white py-3 rounded-lg">
            Sign In
          </Button>

          {/* Forgot Password Link */}
          <div className="text-center mt-4">
            <a
              href="/forgot-password"
              className="text-blue-500 hover:underline"
            >
              Forgot Password?
            </a>
          </div>
        </form>
      </Form>
    </div>
  );
};

export default Login;
