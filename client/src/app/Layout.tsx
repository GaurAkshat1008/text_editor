import React, { Suspense } from "react";
import Navbar from "@/components/common/layout/Navbar";
import Sidebar from "@/components/common/layout/Sidebar";
import LoadingSpinner from "@/components/common/layout/Spinner";
import { Outlet } from "react-router-dom";
import { useAuth } from "@/hooks/useAuth";
import { Alert, AlertTitle, AlertDescription } from "@/components/ui/alert";
import { Button } from "@/components/ui/button";

const Layout: React.FC<{}> = () => {
  const { checkAuth, loading, error, user } = useAuth();

  React.useEffect(() => {
    checkAuth();
  }, [checkAuth]);
  if (loading) {
    return <LoadingSpinner />;
  }

  if (error) {
    return (
      <div>
        <Alert variant="destructive" className="mt-4">
          <AlertTitle>Error</AlertTitle>
          <AlertDescription>Something went wrong.</AlertDescription>
        </Alert>
        <Button
          variant="outline"
          className="mt-2"
          onClick={() => (window.location.href = "/auth/login")}
        >
          Go to Login
        </Button>
      </div>
    );
  }

  return (
    <div className="h-screen flex flex-col">
      <Navbar />
      <div className="flex flex-1 overflow-hidden">
        <Sidebar />
        <main className="flex-1 overflow-y-auto p-4">
          <Suspense fallback={<LoadingSpinner />}>
            <Outlet />
          </Suspense>
        </main>
      </div>
    </div>
  );
};

export default Layout;
